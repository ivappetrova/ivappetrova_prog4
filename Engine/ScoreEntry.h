#ifndef SCORE_ENTRY_H
#define SCORE_ENTRY_H

#include <string>

namespace dae
{
	enum class GameMode { SinglePlayer, Coop, PvP };

	struct ScoreEntry
	{
		std::string name{};
		int score{};
		GameMode mode{ GameMode::SinglePlayer };
	};
}
#endif