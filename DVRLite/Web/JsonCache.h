#pragma once

#include <json/json.h>
#include <map>
#include <filesystem>
namespace DVRLite
{
	class JsonCache
	{
	public:
		JsonCache(uint32_t size);
		Json::Value* Get(const std::string& path);
		void Put(const std::string& path, Json::Value json);
		void Flush(const std::string& path);
		void Preload(const std::filesystem::path& directory);

	private:
		uint32_t size;

		using Cache = std::map<std::string, Json::Value>;
		Cache cache;
	};
}