#include "EndScreen/ScoreManager.h"
#include "Utils/JSONParser.h"
#include <algorithm>
#include <iostream>

namespace dae
{
	void ScoreManager::Load()
	{
		m_SinglePlayerEntries.clear();
		m_CoopEntries.clear();
		m_PvPEntries.clear();

		ScoresFileData data;
		if (!JSONParser::ParseScoresFile(SCORES_PATH, data))
		{
			std::cout << "[ScoreManager] No scores file found, starting fresh."<< std::endl;
			return;
		}

		for (const auto& entryData : data.entries)
		{
			ScoreEntry entry;
			entry.name = entryData.name;
			entry.score = entryData.score;
			if (entryData.mode == "singleplayer")
			{
				entry.mode = GameMode::SinglePlayer;
			}
			else if (entryData.mode == "coop")
			{
				entry.mode = GameMode::Coop;
			}
			else if (entryData.mode == "pvp")
			{
				entry.mode = GameMode::PvP;
			}
			else continue;

			GetEntriesMutable(entry.mode).push_back(entry);
		}
	}

	void ScoreManager::Save() const
	{
		ScoresFileData data;

		auto appendEntries = [&](const std::vector<ScoreEntry>& entries, const std::string& modeStr)
			{
				for (const auto& entryData : entries)
				{
					ScoreEntryData d;
					d.name = entryData.name;
					d.score = entryData.score;
					d.mode = modeStr;
					data.entries.push_back(d);
				}
			};

		appendEntries(m_SinglePlayerEntries, "singleplayer");
		appendEntries(m_CoopEntries, "coop");
		appendEntries(m_PvPEntries, "pvp");

		if (!JSONParser::WriteScoresFile(SCORES_PATH, data))
		{
			std::cerr << "[ScoreManager] Failed to save scores." << std::endl;
		}
	}

	void ScoreManager::AddEntry(const ScoreEntry& entry)
	{
		auto& entries = GetEntriesMutable(entry.mode);
		entries.push_back(entry);

		std::sort(entries.begin(), entries.end(),
			[](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });

		if (static_cast<int>(entries.size()) > MAX_ENTRIES)
		{
			entries.resize(MAX_ENTRIES);
		}
	}

	const std::vector<ScoreEntry>& ScoreManager::GetEntries(GameMode mode) const
	{
		switch (mode)
		{
			case GameMode::SinglePlayer:
			{
				return m_SinglePlayerEntries;
			}
			case GameMode::Coop:
			{
				return m_CoopEntries;
			}
			case GameMode::PvP:
			{
				return m_PvPEntries;
			}
			default:
			{
				return m_SinglePlayerEntries;
			}
		}
	}

	std::vector<ScoreEntry>& ScoreManager::GetEntriesMutable(GameMode mode)
	{
		switch (mode)
		{
			case GameMode::SinglePlayer:
			{
				return m_SinglePlayerEntries;
			}
			case GameMode::Coop:
			{
				return m_CoopEntries;
			}
			case GameMode::PvP:
			{
				return m_PvPEntries;
			}
			default:
			{
				return m_SinglePlayerEntries;
			}
		}
	}
}