// DVRLite.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vector>
#include <unordered_map>
#include "Onvif/Onvif.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"

class DVRLite
{
public:
	class Config
	{
	private:
		mutable std::mutex configMutex;
		const std::string configPath;
		std::string recordPath;
		std::string webPath;

	public:
		Config(const std::string& configPath, const std::string& webPath);

		void Load();
		void Save() const;

		void SetRecordPath(const std::string &recordPath);
		std::string GetRecordPath() const;
		std::string GetWebPath() const;
		std::string GetSourcePath() const;
	};


	DVRLite();

	void AddSource(const Source &source);
	//const std::unordered_set<Source, Source::Hash>& GetSources() const { return sources; }
	std::vector<Source>& GetSources() { return sources; }
	FFmpeg *GetFFmpeg(const Source &source) { return ffmpegs[source.GetName()].get(); }
	FFmpeg* GetFFmpeg(const std::string& source) { return ffmpegs[source].get(); }

	Config& GetConfig();
	const Config& GetConfig() const;
private:

	std::vector<Source> sources;
	std::unordered_map<std::string, std::unique_ptr<FFmpeg>> ffmpegs;

	Config config;
	Onvif onvif;
};