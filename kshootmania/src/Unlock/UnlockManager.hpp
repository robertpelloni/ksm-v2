#pragma once
#include <Siv3D.hpp>

namespace Unlock
{
	class UnlockManager
	{
	private:
		// Stores unlocked item IDs (could be song IDs, difficulties, or items)
		HashSet<String> m_unlockedItems;

		// Stores persistent stats (e.g., total play count, total clear count)
		HashTable<String, int32> m_stats;

		FilePath getSaveFilePath() const;

	public:
		UnlockManager();

		void load();
		void save() const;

		// --- Status Checks ---
		bool isUnlocked(StringView itemId) const;

		int32 getStat(StringView statKey) const;

		// --- Modifiers ---
		// Returns true if this is a newly unlocked item
		bool unlockItem(StringView itemId);

		void incrementStat(StringView statKey, int32 amount = 1);
		void setStat(StringView statKey, int32 value);
	};
}
