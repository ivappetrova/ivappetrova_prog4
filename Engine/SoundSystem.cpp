#include "SoundSystem.h"

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <limits>

#include <SDL3_mixer/SDL_mixer.h>

namespace dae
{
	struct AudioClip
	{
		std::string filePath;
		MIX_Audio* pAudio{ nullptr };
		MIX_Track* pTrack{ nullptr };
		MIX_Mixer* pMixer{ nullptr };
		bool loaded{ false };

		explicit AudioClip(std::string path, MIX_Mixer* mixer)
			: filePath(std::move(path)), pMixer(mixer) {}

		~AudioClip()
		{
			if (pTrack) MIX_DestroyTrack(pTrack);
			if (pAudio) MIX_DestroyAudio(pAudio);
		}

		bool IsLoaded() const { return loaded; }

		void Load()
		{
			pAudio = MIX_LoadAudio(pMixer, filePath.c_str(), false);
			if (!pAudio)
			{
				std::cerr << "[Sound] MIX_LoadAudio failed for '" << filePath << "': " << SDL_GetError() << '\n';
				return;
			}

			pTrack = MIX_CreateTrack(pMixer);
			if (!pTrack)
			{
				std::cerr << "[Sound] MIX_CreateTrack failed: " << SDL_GetError() << '\n';
				MIX_DestroyAudio(pAudio);
				pAudio = nullptr;
				return;
			}

			MIX_SetTrackAudio(pTrack, pAudio);
			loaded = true;
		}

		void Play(float volume)
		{
			if (!loaded) return;
			MIX_SetTrackGain(pTrack, volume);

			SDL_PropertiesID props = SDL_CreateProperties();
			SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, 0);
			MIX_PlayTrack(pTrack, props);
			SDL_DestroyProperties(props);
		}

		void PlayLoop(float volume)
		{
			if (!loaded) return;
			MIX_SetTrackGain(pTrack, volume);

			SDL_PropertiesID props = SDL_CreateProperties();
			SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1); // infinite
			MIX_PlayTrack(pTrack, props);
			SDL_DestroyProperties(props);
		}

		void SetVolume(float volume)
		{
			if (!loaded) return;
			MIX_SetTrackGain(pTrack, volume);
		}
	};

	struct PlayRequest { sound_id id; float volume; bool loop{ false }; };

	struct SoundSystem::Impl
	{
		MIX_Mixer* pMixer{ nullptr };

		std::vector<std::unique_ptr<AudioClip>> clips;
		std::mutex clipsMutex;

		std::queue<PlayRequest> requestQueue;
		std::mutex queueMutex;
		std::condition_variable cv;
		bool quit{ false };
		bool muted{ false };
		sound_id loopingId{ std::numeric_limits<sound_id>::max() };
		float loopingVolume{ 1.f };

		std::thread workerThread;

		Impl()
		{
			if (!MIX_Init())
			{
				std::cerr << "[Sound] MIX_Init failed: " << SDL_GetError() << '\n';
				return;
			}

			pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
			if (!pMixer)
				std::cerr << "[Sound] MIX_CreateMixerDevice failed: " << SDL_GetError() << '\n';

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

			clips.clear(); // destroy tracks/audio before mixer
			if (pMixer) MIX_DestroyMixer(pMixer);
			MIX_Quit();
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
					auto [id, volume, loop] = local.front();
					local.pop();
					ProcessRequest(id, volume, loop);
				}
			}
		}

		void ProcessRequest(sound_id id, float volume, bool loop)
		{
			if (muted) return;

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

			if (!clip->IsLoaded()) return;

			if (loop)
			{
				loopingId = id;
				loopingVolume = volume;
				clip->PlayLoop(volume);
			}
			else
			{
				clip->Play(volume);
			}
		}
	};

	SoundSystem::SoundSystem()
		: m_pImpl(std::make_unique<Impl>()) {}

	SoundSystem::~SoundSystem() = default;

	sound_id SoundSystem::AddSound(const std::string& filePath)
	{
		std::lock_guard lock(m_pImpl->clipsMutex);
		const sound_id id = static_cast<sound_id>(m_pImpl->clips.size());
		m_pImpl->clips.push_back(std::make_unique<AudioClip>(filePath, m_pImpl->pMixer));
		return id;
	}

	void SoundSystem::Play(sound_id id, float volume)
	{
		{
			std::lock_guard lock(m_pImpl->queueMutex);
			m_pImpl->requestQueue.push({ id, volume, false });
		}
		m_pImpl->cv.notify_one();
	}

	void SoundSystem::PlayLoop(sound_id id, float volume)
	{
		{
			std::lock_guard lock(m_pImpl->queueMutex);
			m_pImpl->requestQueue.push({ id, volume, true });
		}
		m_pImpl->cv.notify_one();
	}

	void SoundSystem::SetMuted(bool muted)
	{
		{
			std::lock_guard lock(m_pImpl->queueMutex);
			m_pImpl->muted = muted;
		}

		std::lock_guard lock(m_pImpl->clipsMutex);
		if (m_pImpl->loopingId != std::numeric_limits<sound_id>::max() &&
			m_pImpl->loopingId < m_pImpl->clips.size())
		{
			auto* clip = m_pImpl->clips[m_pImpl->loopingId].get();
			clip->SetVolume(muted ? 0.f : m_pImpl->loopingVolume);
		}
	}
}