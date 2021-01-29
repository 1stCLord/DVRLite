#include "JsonCache.h"
#include <fstream>
#include <thread>

JsonCache::JsonCache(uint32_t size) : size(size){}

Json::Value* JsonCache::Get(const std::string& path)
{
	Cache::iterator found = cache.find(path);
	if (found != cache.end())
		return &found->second;

	std::ifstream file(path);
	if (file.is_open())
	{
		if (cache.size() >= size)
			cache.erase(cache.begin());

		Json::Value &json = cache[path];
		file >> json;
		return &json;
	}
	return nullptr;

}

void JsonCache::Flush(const std::string& path)
{
	Cache::iterator found = cache.find(path);
	if (found != cache.end())
	{
		std::ofstream file(path);
		if (file.is_open())
			file << found->second;
	}
}

void JsonCache::Preload(const std::filesystem::path& directory)
{
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.path().extension() == ".json")
			Get(entry.path().string());
	}
}