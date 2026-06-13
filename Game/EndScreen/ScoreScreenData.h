#ifndef SCORE_SCREEN_DATA_H
#define SCORE_SCREEN_DATA_H

#include "ScoreEntry.h"
#include <vector>

namespace dae
{
	struct ScoreScreenData
	{
		GameMode mode{ GameMode::SinglePlayer };

		// SP/Coop: one entry per player slot
		std::vector<int> playerScores;

		// PvP only
		// 0 = P1, 1 = P2
		int winnerIndex{ -1 }; 
	};
}
#endif