#include <commands/ExportAnimation.h>
#include <core/MathHelper.h>

#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>


using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

static bool BeginConversion(const string& importSkeleton, const string& importHKX, const string& cacheFilePath, const string& behaviorFolder, const string& exportPath);
static void InitializeHavok();
static void CloseHavok();


ExportAnimation::ExportAnimation()
{
}

ExportAnimation::~ExportAnimation()
{
}

string ExportAnimation::GetName() const
{
	return "exportanimation";
}

string ExportAnimation::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_hkx> <path_to_hkx_animation> [--b=<path_to_behavior_folder>] [--c=<path_to_cache_file>] [--n=<path_to_additional_nifs>] [--e=<path_to_export>]\r\n";

	const char help[] =
		R"(Converts an HKX animation to FBX. Requires a preexisting HKX skeleton
		
		Arguments:
			<path_to_skeleton_hkx> the animation skeleton in hkx format
			<path_to_hkx_animation> the FBX animation to convert
			--c=<path_to_cache_file>, --cache <path_to_cache_file> necessary to extract root motion into animations
			--b=<path_to_behavior_folder>, --behavior <path_to_behavior_folder> necessary to extract root motion
			--e=<path_to_export>, --export-dir <path_to_export>  optional export path

		)";
	return usage + help;
}

string ExportAnimation::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}

bool ExportAnimation::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importHKX, importSkeleton, exportPath, cacheFilePath, behaviorFolder;

	importSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	importHKX = parsedArgs["<path_to_hkx_animation>"].asString();
	if (parsedArgs["--c"].isString())
		cacheFilePath = parsedArgs["--c"].asString();
	if (parsedArgs["--b"].isString())
		behaviorFolder = parsedArgs["--b"].asString();
	if (parsedArgs["--e"].isString())
		exportPath = parsedArgs["--e"].asString();

	InitializeHavok();
	BeginConversion(importSkeleton, importHKX, cacheFilePath, behaviorFolder, exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(
	const string& importSkeleton, 
	const string& importHKX,
	const string& cacheFilePath,
	const string& behaviorFolder,
	const string& exportPath
) {
	// Get Import Skeleton
	if (!fs::exists(importSkeleton) || !fs::is_regular_file(importSkeleton)) {
		Log::Error("Invalid file: %s", importSkeleton.c_str());
		return false;
	}

	// Gather animation files
	vector<fs::path> fbxs;
	if (fs::is_regular_file(importHKX)) {
		fbxs.push_back(importHKX);
	}
	else if (fs::is_directory(importHKX)) {
		find_files(importHKX, ".hkx", fbxs);
	}
	else {
		Log::Error("Invalid path: %s", importHKX.c_str());
		return false;
	}

	// Create output directory
	fs::path outputDir = fs::path(exportPath);
	fs::create_directories(outputDir);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}

	// Get Root Motion
	StaticCacheEntry entry;
	std::map< fs::path, RootMovement> map;
	if (fs::exists(cacheFilePath) && !fs::is_directory(cacheFilePath) &&
		fs::exists(behaviorFolder) && fs::is_directory(behaviorFolder))
	{
		AnimationCache::get_entries(entry, cacheFilePath);
		Log::Info("Loaded animation cache info from %s", cacheFilePath.c_str());
		HKXWrapper wrap;
		wrap.GetStaticClipsMovements(
			fbxs,
			entry,
			behaviorFolder,
			map
		);
	}

	// Export each animation
	for (const auto& fbx : fbxs) {
		Log::Info("Exporting: %s, using current_dir", fbx.string().c_str());
		FBXWrangler wrangler;
		wrangler.NewScene();
		FbxNode* skeleton_root = NULL;
		vector<FbxProperty> floats;
		vector<FbxNode*> ordered_skeleton = wrangler.importExternalSkeleton(importSkeleton, "", floats);
		Log::Info("Exporting: %s, using current_dir", fbx.string().c_str());

		auto root_movement = map.find(fbx);
		if (root_movement != map.end())
		{
			wrangler.importAnimationOnSkeleton(
				fbx.string(),
				ordered_skeleton,
				floats,
				root_movement->second);
		}
		else {
			wrangler.importAnimationOnSkeleton(
				fbx.string(),
				ordered_skeleton,
				floats,
				RootMovement());
		}
		Log::Info("Exporting: %s, using current_dir", fbx.string().c_str());

		fs::path out_path = outputDir / fs::path(fbx).filename().replace_extension(".fbx");
		fs::create_directories(out_path.parent_path());
		wrangler.ExportScene(out_path.string().c_str());
	}

	return true;
}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}