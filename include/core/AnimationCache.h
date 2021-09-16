#pragma once

#include "stdafx.h"

#include <core/hkfutils.h>
#include <core/log.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>

#include <set>
#include <filesystem>
#include <memory>
#include <algorithm>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

/**
* Created by Bet on 10.03.2016.
*/
class HkCRC {
	//int crc_order = 32;
	//std::string crc_poly = "4C11DB7";
	//std::string initial_value = "000000";
	//std::string final_value = "000000";

	static int reflectByte(int c);
	static void reflect(int* crc, int bitnum, int startLSB);
public:
	
	static std::string compute(std::string input);
};

struct CacheEntry
{
	string name;
	AnimData::ProjectBlock& block;
	AnimData::ProjectDataBlock& movements;

	CacheEntry() : block(AnimData::ProjectBlock()) , movements(AnimData::ProjectDataBlock()){}
	CacheEntry(const string& name, AnimData::ProjectBlock& block, AnimData::ProjectDataBlock& movements) : name(name), block(block), movements(movements) {}

	bool hasCache() { return block.getHasAnimationCache(); }

	std::vector<std::string> getEvents(const std::string& clip_name) {
		if (hasCache())
		{
			auto it = find_if(block.getClips().begin(), block.getClips().end(), [&clip_name](AnimData::ClipGeneratorBlock& block) {return block.getName() == clip_name; });
			if (it != block.getClips().end())
			{
				return it->getEvents().getStrings();
			}
		}
		return {};
	}

	std::vector<AnimData::ClipMovementData>& getMovements() {
		return movements.getMovementData();
	}

	std::string findMovement(const std::string& clip_name)
	{
		if (hasCache())
		{
			auto it = find_if(block.getClips().begin(), block.getClips().end(), [&clip_name](AnimData::ClipGeneratorBlock& block) {return block.getName() == clip_name; });
			if (it != block.getClips().end())
			{
				if ((size_t)it->getCacheIndex() < movements.getMovementData().size())
					return movements.getMovementData()[it->getCacheIndex()].getBlock();
			}
		}
		return {};
	}

	virtual void none() {}
};

struct StaticCacheEntry
{
	string name;
	AnimData::ProjectBlock block;
	AnimData::ProjectDataBlock movements;

	StaticCacheEntry() {}
	StaticCacheEntry(const string& name, AnimData::ProjectBlock& block, AnimData::ProjectDataBlock& movements) : name(name), block(block), movements(movements) {}

	bool hasCache() { return block.getHasAnimationCache(); }

	virtual void none() {}
};

struct CreatureCacheEntry : public CacheEntry
{
	AnimData::ProjectAttackListBlock& sets;

	CreatureCacheEntry() : sets(AnimData::ProjectAttackListBlock()) {}
	CreatureCacheEntry(const string& name, AnimData::ProjectBlock& block, AnimData::ProjectDataBlock& movements, AnimData::ProjectAttackListBlock& sets) :
		CacheEntry(name, block, movements), sets(sets) {}

	vector<string> findProjectFile(const std::string& file) {
		std::vector<string> out;
		std::string to_crc = fs::path(file).filename().replace_extension("").string();
		transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);
		long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
		string crc_str = to_string(crc);

		auto blocks = sets.getProjectAttackBlocks();
		auto projectFiles = sets.getProjectFiles().getStrings();
		for (size_t i = 0; i < blocks.size(); i++) {
			auto& crc32 = blocks[i].getCrc32Data().getStrings();
			for (auto& entry : crc32) {
				if (entry == crc_str) {
					out.push_back(projectFiles[i]);
				}
			}
		}
		return out;
	}

	vector<string> getProjectSetFiles()
	{
		return sets.getProjectFiles().getStrings();
	}

	vector<string> getProjectSetEvents(const std::string& file) {
		auto projectFiles = sets.getProjectFiles().getStrings();
		auto blocks = sets.getProjectAttackBlocks();
		size_t set_index = distance(projectFiles.begin(), find(projectFiles.begin(), projectFiles.end(), file));
		return blocks[set_index].getSwapEventsList().getStrings();
	}

	AnimData::HandVariableData& getProjectSetVariables(const std::string& file) {
		auto projectFiles = sets.getProjectFiles().getStrings();
		auto blocks = sets.getProjectAttackBlocks();
		size_t set_index = distance(projectFiles.begin(), find(projectFiles.begin(), projectFiles.end(), file));
		return blocks[set_index].getHandVariableData();
	}

	pair<vector<string>, AnimData::HandVariableData> getProjectSetInfo(const std::string& project_set_key) {
		auto projectFiles = sets.getProjectFiles().getStrings();
		auto blocks = sets.getProjectAttackBlocks();
		size_t set_index = distance(projectFiles.begin(), find(projectFiles.begin(), projectFiles.end(), project_set_key));
		return { 
			blocks[set_index].getSwapEventsList().getStrings(),
			blocks[set_index].getHandVariableData() 
		};
	}

};

struct AnimationCache {

	static constexpr const char* blocks_regex = "meshes\\\\animationdata\\\\.[^\\]+txt";
	static constexpr const char* movements_blocks_regex = "meshes\\\\animationdata\\\\boundanims\\\\.[^\\]+txt";
	static constexpr const char* attack_blocks_regex = "meshes\\\\animationsetdata\\\\.+\\\\.+txt";


	static constexpr const char* animation_data_folder = "animationdata";
	static constexpr const char* animation_data_merged_file = "animationdatasinglefile.txt";
	static constexpr const char* animation_set_data_folder = "animationsetdata";
	static constexpr const char* animation_set_data_merged_file = "animationsetdatasinglefile.txt";

	//project, clip -> movement
	typedef pair<string, string> movement_key_t;
	map< movement_key_t, AnimData::ClipMovementData> movements_map;

	const AnimData::ClipMovementData& getMovement(string project, string clip) {
		return movements_map.at({ project, clip });
	}

	enum event_type_t
	{
		idle,
		attack
	};

	struct event_info_t
	{
		event_type_t type;
		vector<AnimData::HandVariableData::Data> animation_set;
	};

	//project, event -> type
	typedef pair<string, string> eventset_key_t;
	multimap< eventset_key_t, event_info_t> events_map;

	vector<event_info_t> getEventsInfo(string project, string anim_event) {
		vector<event_info_t> out;
		auto entries = events_map.equal_range({ project, anim_event });
		for (auto it = entries.first; it != entries.second; it++) {
			out.push_back(it->second);
		}
		return out;
	}

	static inline bool iequals(const string& a, const string& b)
	{
		return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b) {
				return tolower(a) == tolower(b);
			});
	}

	vector<CreatureCacheEntry> creature_entries;
	vector<CacheEntry>		   misc_entries;

	AnimData::AnimDataFile animationData;
	AnimData::AnimSetDataFile animationSetData;

	map<string, CacheEntry*> projects_index;

	CacheEntry* find(const string& name);
	string project_at(size_t index) const;
	size_t getNumCreatureProjects();
	size_t getNumProjects();

	AnimationCache(const fs::path& workspacePath);
	AnimationCache(const fs::path& animationDataPath, const  fs::path& animationSetDataPath);
	AnimationCache(const string& animationDataContent, const string& animationSetDataContent);

	CreatureCacheEntry* cloneCreature(const std::string& source_project, const std::string& destination_project);

	void save(const fs::path& animationDataPath, const  fs::path& animationSetDataPath);
	void save_creature(const string& project, CacheEntry* project_entry, const fs::path& animationDataPath, const  fs::path& animationSetDataPath);

	void rebuildIndex();
	void build(const string& animationDataContent, const string& animationSetDataContent);



	static void get_entries(
		StaticCacheEntry& entry,
		const string& cacheFile
	);

	static void create_entry(
		CacheEntry& entry,
		const ckcmd::BSA::BSAFile& bsa_file,
		const string& name);

	static void create_creature_entry(
		CreatureCacheEntry& entry,
		const ckcmd::BSA::BSAFile& bsa_file,
		const string& name);

	void check_from_bsa(const ckcmd::BSA::BSAFile& bsa_file, const std::vector<string>& actors, const std::vector<string>& misc);

	void printInfo();

	//void checkInfo() {
	//	for (const auto& pair : set_data_map) {
	//		Log::Info("\tID:%d\tName: %s", pair.second, pair.first.c_str());

	//		int data_index = data_map[pair.first];
	//		int set_data_index = set_data_map[pair.first];

	//		AnimData::ProjectBlock& this_data = animationData.getProjectBlock(data_index);
	//		AnimData::ProjectDataBlock& this_movement_data = animationData.getprojectMovementBlock(data_index);
	//		AnimData::ProjectAttackListBlock& this_set_data = animationSetData.getProjectAttackBlock(set_data_index);
	//		Log::Info("\t\tHavok Files:%d", this_data.getProjectFiles().getStrings().size());
	//		list<string> projects = this_data.getProjectFiles().getStrings();
	//		for (auto& havok_file : projects)
	//			Log::Info("\t\t\t%s", havok_file.c_str());

	//		//Check CRC clips number
	//		size_t movements = this_movement_data.getMovementData().size();
	//		set<string> paths;
	//		
	//		for (auto& block : this_set_data.getProjectAttackBlocks())
	//		{
	//			auto& strings = block.getCrc32Data().getStrings();
	//			std::list<std::string>::iterator it;
	//			int i = 0;
	//			string this_path;
	//			for (it = strings.begin(); it != strings.end(); ++it) {
	//				if (i % 3 == 0)
	//					this_path = *it;
	//				if (i % 3 == 1)
	//					paths.insert(this_path + *it);
	//				i++;
	//			}
	//		}
	//		size_t crcs = paths.size();
	//		if (movements != crcs)
	//			Log::Info("Warning: unaddressed movement data!");

	//	}
	//}


	//bool hasCreatureProject (const string& project_name) {
	//	return set_data_map.find(project_name) != set_data_map.end();
	//}

};