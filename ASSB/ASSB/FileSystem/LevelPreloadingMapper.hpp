#pragma once
#include "FileSystem\File.hpp"
#include <string>
#include <unordered_map>
#include <queue>
#include "Globals.hpp"


namespace FileSystem
{
	// Preloads a given level in the engine
	class LevelPreloadingMapper
	{
	public:
		// Static Functions
		static void LoadFromFile(std::string filepath);
		static void LevelFromFile(std::string filepath, bool automaticOffset = true, long long offsetX = 0);
		static long long CurrentOffset;
		static bool Associate(std::string tag, std::string path);
		static std::string Retrieve(std::string tag);
		static const std::unordered_map<std::string, std::string> &DumpTags();
		static void NukeObjects();
		static void ResolveNukes();
		static void CheckOldestLoaded();

	private:
		// Variables
		static bool wasNuked_;
		const static long long leftCullUnits_;
		static bool ParseLine(std::string line, long long offsetX, int &width);
		static std::unordered_map<std::string, std::string> associatedStrings_;
		static std::queue<ASSB::Globals::ObjectID> loadedHistory_;

		// Private member functions
		static void resetPosition();
	};
}
