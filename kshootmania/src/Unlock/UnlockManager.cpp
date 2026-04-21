#include "UnlockManager.hpp"
#include "Common/FsUtils.hpp"
#include "Ini/ConfigIni.hpp"

namespace Unlock
{
	UnlockManager::UnlockManager()
	{
		load();
	}

	FilePath UnlockManager::getSaveFilePath() const
	{
		// Use player profile prefix if implemented, otherwise global unlock file
		const String playerStr = ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer, U"player1");
		return FileSystem::PathAppend(FsUtils::AppDataDirectoryPath(), U"save/{}_unlocks.json"_fmt(playerStr));
	}

	void UnlockManager::load()
	{
		const FilePath path = getSaveFilePath();

		if (!FileSystem::Exists(path))
		{
			// Initialize with empty sets
			return;
		}

		JSON json = JSON::Load(path);
		if (!json)
		{
			Logger << U"[ksm error] Failed to load unlock data: " << path;
			return;
		}

		// Load Unlocked Items
		m_unlockedItems.clear();
		if (json.hasElement(U"unlocked_items") && json[U"unlocked_items"].isArray())
		{
			for (const auto& item : json[U"unlocked_items"].arrayView())
			{
				m_unlockedItems.insert(item.getString());
			}
		}

		// Load Stats
		m_stats.clear();
		if (json.hasElement(U"stats") && json[U"stats"].isObject())
		{
			for (const auto& [key, value] : json[U"stats"].objectView())
			{
				m_stats[key] = value.getOr<int32>(0);
			}
		}
	}

	void UnlockManager::save() const
	{
		JSON json;

		// Save Unlocked Items
		Array<String> unlockedArray;
		for (const auto& item : m_unlockedItems)
		{
			unlockedArray.push_back(item);
		}
		json[U"unlocked_items"] = unlockedArray;

		// Save Stats
		for (const auto& [key, value] : m_stats)
		{
			json[U"stats"][key] = value;
		}

		const FilePath path = getSaveFilePath();

		// Ensure directory exists
		FileSystem::CreateDirectories(FileSystem::ParentPath(path));

		if (!json.save(path))
		{
			Logger << U"[ksm error] Failed to save unlock data: " << path;
		}
	}

	bool UnlockManager::isUnlocked(StringView itemId) const
	{
		return m_unlockedItems.contains(itemId);
	}

	int32 UnlockManager::getStat(StringView statKey) const
	{
		if (m_stats.contains(statKey))
		{
			return m_stats.at(statKey);
		}
		return 0;
	}

	bool UnlockManager::unlockItem(StringView itemId)
	{
		if (m_unlockedItems.contains(itemId))
		{
			return false; // Already unlocked
		}

		m_unlockedItems.insert(itemId);
		save();
		return true; // Newly unlocked
	}

	void UnlockManager::incrementStat(StringView statKey, int32 amount)
	{
		m_stats[statKey] += amount;
		save();
	}

	void UnlockManager::setStat(StringView statKey, int32 value)
	{
		m_stats[statKey] = value;
		save();
	}
}
