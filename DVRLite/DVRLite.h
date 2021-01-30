// DVRLite.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <unordered_set>
#include <unordered_map>
#include "Onvif/Onvif.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"
#include "Web/JsonCache.h"

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
		Config(const std::string& configPath, const std::string& webPath, uint16_t port);

		void Load();
		void Save() const;

		void SetRecordPath(const std::string &recordPath);
		std::string GetRecordPath() const;
		std::string GetWebPath() const;
		std::string GetSourcePath() const;
		std::string GetLogPath() const;
		void SetPort(uint16_t port);
		uint16_t GetPort()const;

		static uint16_t Port;
	};

	using SourceSet = std::unordered_set <Source, Source::Hash>;


	DVRLite(const std::string& configPath = "config.json", const std::string& webPath = "web/", uint16_t port = 8000);

	void AddSource(const Source &source);
	void RemoveSource(const std::string& source);

	const Source& GetSource(const std::string &sourceName) const;
	SourceSet& GetSources() { return sources; }
	FFmpeg* GetFFmpeg(const Source& source) { return GetFFmpeg(source.GetName()); }
	FFmpeg* GetFFmpeg(const Source& source) const { return GetFFmpeg(source.GetName()); }
	FFmpeg* GetFFmpeg(const std::string& source) { return ffmpegs[source].get(); }
	FFmpeg* GetFFmpeg(const std::string& source) const { return (*ffmpegs.find(source)).second.get();}

	JsonCache& GetCache();

	Config& GetConfig();
	const Config& GetConfig() const;

	static void Log(const std::string& logline);
private:

	SourceSet sources;
	std::unordered_map<std::string, std::unique_ptr<FFmpeg>> ffmpegs;

	Config config;
	Onvif onvif;
	JsonCache cache;

	static std::ofstream logfile;
};