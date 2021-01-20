// DVRLite.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <filesystem>
#include <json/json.h>
#include "oatpp/web/server/api/ApiController.hpp"

using QueryParams = oatpp::web::protocol::http::QueryParams;

class DVRLite
{
public:

	struct Source
	{
		std::string name;
		std::string address;
		std::string username;
		std::string password;
		uint16_t duration;
		uint16_t quota;
		std::vector<std::string> triggers;

		Source(){}
		Source(const std::string &name, const std::string &address, uint16_t duration, uint16_t quota, const std::vector<std::string> &triggers) :
			name(name),
			address(address),
			duration(duration),
			quota(quota),
			triggers(triggers)
		{
		}
		Source(const std::filesystem::path& path) { Load(path); }
		Source(const QueryParams &queryParams);

		void Save(const std::filesystem::path &path) const;
		void Load(const std::filesystem::path &path);
	};

	DVRLite();

	void AddSource(const Source &source);
	const std::vector<Source>& GetSources() const { return sources; }
private:

	std::vector<Source> sources;
};