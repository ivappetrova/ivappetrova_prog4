#include "SoundSystem.h"

// Windows MCI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

namespace dae
{
	struct AudioClip
	{
		std::string filePath;
		std::string alias;
		bool loaded{ false };

		explicit AudioClip(std::string path, std::string al)
			: filePath(std::move(path)), alias(std::move(al)) 
		{
		}

		~AudioClip()
		{
			if (loaded)
			{
				std::string cmd = "close " + alias;
				mciSendStringA(cmd.c_str(), nullptr, 0, nullptr);
			}
		}

		bool IsLoaded() const 
		{ 
			return loaded; 
		}

		void Load()
		{
			std::string cmd = "open \"" + filePath + "\" type mpegvideo alias " + alias;
			MCIERROR err = mciSendStringA(cmd.c_str(), nullptr, 0, nullptr);

			if (err != 0)
			{
				cmd = "open \"" + filePath + "\" type waveaudio alias " + alias;
				err = mciSendStringA(cmd.c_str(), nullptr, 0, nullptr);
			}

			if (err != 0)
			{
				char errBuf[256];
				mciGetErrorStringA(err, errBuf, sizeof(errBuf));
				std::cerr << "[Sound] MCI open failed for '" << filePath << "': " << errBuf << '\n';
				return;
			}
			loaded = true;
		}

		void Play(float volume)
		{
			if (!loaded) return;

			std::string seekCmd = "seek " + alias + " to start";
			mciSendStringA(seekCmd.c_str(), nullptr, 0, nullptr);

			const int mciVolume = static_cast<int>(volume * 1000);
			std::string volCmd = "setaudio " + alias + " volume to " + std::to_string(mciVolume);
			mciSendStringA(volCmd.c_str(), nullptr, 0, nullptr);

			std::string playCmd = "play " + alias;
			MCIERROR err = mciSendStringA(playCmd.c_str(), nullptr, 0, nullptr);
			if (err != 0)
			{
				char errBuf[256];
				mciGetErrorStringA(err, errBuf, sizeof(errBuf));
				std::cerr << "[Sound] MCI play failed for '" << alias << "': " << errBuf << '\n';
			}
		}
	};

	// -------------------------------------------------------------------------
	// Play request — placed in the queue by Play()
	// -------------------------------------------------------------------------
	struct PlayRequest
	{
		sound_id id;
		float    volume;
	};

	// -------------------------------------------------------------------------
	// Impl — all MCI state + threading lives here
	// -------------------------------------------------------------------------
	struct SoundSystem::Impl
	{
		std::vector<std::unique_ptr<AudioClip>> clips;
		std::mutex                              clipsMutex;

		std::queue<PlayRequest> requestQueue;
		std::mutex              queueMutex;
		std::condition_variable cv;
		bool                    quit{ false };
		unsigned int            aliasCounter{ 0 };

		std::thread workerThread;

		Impl()
		{
			workerThread = std::thread(&Impl::WorkerLoop, this);
		}

		~Impl()
		{
			{
				std::lock_guard lock(queueMutex);
				quit = true;
			}
			cv.notify_one();
			if (workerThread.joinable())
				workerThread.join();
		}

		std::string MakeAlias()
		{
			return "sound" + std::to_string(aliasCounter++);
		}

		void WorkerLoop()
		{
			while (true)
			{
				std::unique_lock lock(queueMutex);
				cv.wait(lock, [this] { return !requestQueue.empty() || quit; });

				if (quit && requestQueue.empty())
					break;

				std::queue<PlayRequest> local;
				std::swap(local, requestQueue);
				lock.unlock();

				while (!local.empty())
				{
					auto [id, volume] = local.front();
					local.pop();
					ProcessRequest(id, volume);
				}
			}
		}

		void ProcessRequest(sound_id id, float volume)
		{
			AudioClip* clip = nullptr;
			{
				std::lock_guard lock(clipsMutex);
				if (id >= clips.size())
				{
					std::cerr << "[Sound] Invalid sound_id " << id << '\n';
					return;
				}
				clip = clips[id].get();
			}

			if (!clip->IsLoaded())
				clip->Load();

			if (!clip->IsLoaded())
				return;

			clip->Play(volume);
		}
	};

	// -------------------------------------------------------------------------
	// SoundSystem public API
	// -------------------------------------------------------------------------

	SoundSystem::SoundSystem()
		: m_pImpl(std::make_unique<Impl>())
	{}

	SoundSystem::~SoundSystem() = default;

	sound_id SoundSystem::AddSound(const std::string& filePath)
	{
		std::lock_guard lock(m_pImpl->clipsMutex);
		const sound_id id = static_cast<sound_id>(m_pImpl->clips.size());
		m_pImpl->clips.push_back(std::make_unique<AudioClip>(filePath, m_pImpl->MakeAlias()));
		return id;
	}

	void SoundSystem::Play(sound_id id, float volume)
	{
		{
			std::lock_guard lock(m_pImpl->queueMutex);
			m_pImpl->requestQueue.push({ id, volume });
		}
		m_pImpl->cv.notify_one();
	}
}