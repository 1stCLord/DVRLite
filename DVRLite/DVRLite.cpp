// DVRLite.cpp : Defines the entry point for the application.
//

#include "DVRLite.h"
#include "Web/MediaController.hpp"
#include "json/json.h"
#include <filesystem>
#include <fstream>

uint16_t DVRLite::Config::Port = 8000;
std::ofstream DVRLite::logfile;

int main(int argc, const char* argv[]) 
{
    std::string configPath = argc >= 2 ? argv[1] : "config.json";
    std::string webPath = argc >= 3 ? argv[2] : "../../../../web/";
    uint16_t port = argc >= 4 ? std::stoi(argv[3]) : DVRLite::Config::Port;

    DVRLite dvrlite = DVRLite(configPath, webPath, port);
    MediaController::run(&dvrlite);

    return 0;
}

DVRLite::DVRLite(const std::string& configPath, const std::string& webPath, uint16_t port) : config(configPath, webPath, port), cache(100000)
{
    logfile = std::ofstream(std::filesystem::path(configPath).parent_path() / "log.txt");
    config.Load();

    //load the sources
    if (std::filesystem::is_directory(config.GetSourcePath()))
    {
        for (std::filesystem::directory_entry directory : std::filesystem::directory_iterator(config.GetSourcePath()))
        {
            std::filesystem::path configPath = directory.path() / "config.json";
            if (directory.is_directory() && std::filesystem::is_regular_file(configPath))
            {
                std::pair<std::unordered_set<Source>::iterator, bool> result = sources.insert(Source(configPath));
                const Source& source = *(result.first);
                ffmpegs[source.GetName()] = std::make_unique<FFmpeg>(source, this);
            }
        }
    }
    //get the onvif info
    onvif.Init(this);
}

void DVRLite::AddSource(const Source &source)
{
    Log("Adding source " + source.GetUsername());
    std::pair<SourceSet::iterator, bool> result = sources.insert(source);
    const Source& ourSource = *(result.first);
    ffmpegs[source.GetName()] = std::make_unique<FFmpeg>(ourSource, this);
    onvif.Add(ourSource);
    ourSource.Save(std::filesystem::path(config.GetSourcePath()) / source.GetName() / std::string("config.json"));
}

void DVRLite::RemoveSource(const std::string& sourceName)
{
    Log("Removing source " + sourceName);
    ffmpegs[sourceName].reset();
    onvif.Remove(sourceName);
    sources.erase(std::find_if(sources.begin(), sources.end(), [&](const Source& source) {return source.GetName() == sourceName; }));
}

const Source& DVRLite::GetSource(const std::string &sourceName) const
{
    return *std::find_if(sources.begin(), sources.end(), [&](const Source& source) {return source.GetName() == sourceName; });
}

JsonCache& DVRLite::GetCache()
{
    return cache;
}

DVRLite::Config& DVRLite::GetConfig()
{
    return config;
}

const DVRLite::Config& DVRLite::GetConfig() const
{
    return config;
}

void DVRLite::Log(const std::string& logline)
{
    std::string tsLogline = to_string(std::chrono::system_clock::now(), std::string(DATESTRINGFORMAT) + ": ") + logline;
    std::cout << tsLogline << std::endl;
    if(logfile.is_open())
        logfile << tsLogline << std::endl;
}

DVRLite::Config::Config(const std::string& configPath, const std::string &webPath, uint16_t port) : 
    configPath(configPath), 
    webPath(webPath)
{
    Port = port;
}

void DVRLite::Config::Load()
{
    if (std::filesystem::is_regular_file(configPath))
    {
        std::ifstream file(configPath);
        if (file.is_open())
        {
            Json::Value config;
            file >> config;

            std::lock_guard lock(configMutex);
            std::string newRecordPath = config["recordPath"].asString();
            if (!newRecordPath.empty())
                recordPath = newRecordPath;
            uint16_t port = config["port"].asUInt();
            if (port != 0)
                Port = port;

            std::stringstream jsonString;
            jsonString << config;
            DVRLite::Log("Config Loaded:\n " + jsonString.str());
        }
    }
}

void DVRLite::Config::Save() const
{
    Json::Value source;
    {
        std::lock_guard lock(configMutex);
        source["recordPath"] = recordPath;
        source["port"] = Port;
    }
    std::filesystem::path fullpath(configPath);
    std::filesystem::create_directories(fullpath.parent_path());
    std::ofstream file(configPath);
    file << source;

    std::stringstream jsonString;
    jsonString << source;
    DVRLite::Log("Config Saved:\n " + jsonString.str());
}

void DVRLite::Config::SetRecordPath(const std::string& recordPath)
{
    {
        std::lock_guard lock(configMutex);
        this->recordPath = recordPath;
    }
    Save();
}

std::string DVRLite::Config::GetRecordPath() const
{
    std::lock_guard lock(configMutex);
    return recordPath;
}

std::string DVRLite::Config::GetWebPath() const
{
    std::lock_guard lock(configMutex);
    return webPath;
}

std::string DVRLite::Config::GetSourcePath() const
{
    std::lock_guard lock(configMutex);
    return (std::filesystem::path(configPath).parent_path() / "sources").string();
}

uint16_t DVRLite::Config::GetPort() const
{
    std::lock_guard lock(configMutex);
    return Port;
}

void DVRLite::Config::SetPort(uint16_t port)
{
    std::lock_guard lock(configMutex);
    this->Port = port;
}
