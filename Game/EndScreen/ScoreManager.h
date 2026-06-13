#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include "ScoreEntry.h"
#include <vector>
#include <string>

namespace dae
{
	class ScoreManager final
	{
	public:
		static constexpr int MAX_ENTRIES{ 10 };
		static constexpr const char* SCORES_PATH {"Data/scores.json"};

		static ScoreManager& GetInstance()
		{
			static ScoreManager instance;
			return instance;
		}

		void Load();
		void Save() const;
		void AddEntry(const ScoreEntry& entry);
		const std::vector<ScoreEntry>& GetEntries(GameMode mode) const;

	private:
		ScoreManager() = default;

		std::vector<ScoreEntry> m_SinglePlayerEntries;
		std::vector<ScoreEntry> m_CoopEntries;
		std::vector<ScoreEntry> m_PvPEntries;

		std::vector<ScoreEntry>& GetEntriesMutable(GameMode mode);
	};
}
#endif