#ifndef MINIGIN_H
#define MINIGIN_H

#include <string>
#include <functional>
#include <filesystem>
#include <chrono>

namespace dae
{
	class Minigin final
	{
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		int GetWindowWidth()  const { return m_WindowWidth; }
		int GetWindowHeight() const { return m_WindowHeight; }

		// Rule of 5
		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

	private:
		int m_WindowWidth{1024};
		int m_WindowHeight{700};


		bool m_Quit{};
		std::chrono::high_resolution_clock::time_point m_LastTime{ std::chrono::high_resolution_clock::now() };

		float m_Lag{};
		static constexpr float m_FixedTimeStep{ 1.0f / 60.0f };
	};
}
#endif