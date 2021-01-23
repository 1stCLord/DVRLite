// DVRLite.cpp : Defines the entry point for the application.
//

#include "DVRLite.h"
#include "Web/MediaController.hpp"
#include "json/json.h"
#include <filesystem>
#include <fstream>

int main(int argc, const char* argv[]) 
{
    //using namespace std::chrono_literals;
    /*using namespace std::chrono_literals;
    FFmpeg ffmpeg = FFmpeg("rtmp://192.168.0.75/bcs/channel0_main.bcs?channel=0&stream=0&user=admin&password=gibraltar", false, true);
    ffmpeg.Record(10s, "video.mp4");
    std::this_thread::sleep_for(8s);
    ffmpeg.Record(10s, "video.mp4");
    std::this_thread::sleep_for(8s);
    ffmpeg.Record(14s, "video.mp4");*/

    /*while(ffmpeg.IsRecording())
        std::this_thread::sleep_for(1s);*/
    DVRLite dvrlite;
    /*Source source = Source("name", "http://192.168.0.75:8000/onvif/device_service", 0, 0, std::unordered_set<std::string>());
    PullPointSubscription pullpoint(source, []() {});
    std::this_thread::sleep_for(1000s);*/
    MediaController::run(&dvrlite);

    return 0;
}

DVRLite::DVRLite() : config("config.json", "F:/Projects/DVRLite/web/")
{
    config.Load();

    //load the sources
    if (std::filesystem::is_directory(config.GetSourcePath()))
    {
        for (std::filesystem::directory_entry directory : std::filesystem::directory_iterator(config.GetSourcePath()))
        {
            std::filesystem::path configPath = directory.path() / "config.json";
            if (directory.is_directory() && std::filesystem::is_regular_file(configPath))
            {
                sources.push_back(Source(configPath));
                Source& source = sources.back();
                ffmpegs[source.GetName()] = std::make_unique<FFmpeg>(source);
            }
        }
    }
    //get the onvif info
    onvif = Onvif(this);
}

void DVRLite::AddSource(const Source &source)
{
    //TODO remove existing
    sources.push_back(source);
    source.Save(std::filesystem::path(config.GetSourcePath()) / source.GetName() / std::string("config.json"));
}

DVRLite::Config& DVRLite::GetConfig()
{
    return config;
}

const DVRLite::Config& DVRLite::GetConfig() const
{
    return config;
}

DVRLite::Config::Config(const std::string& configPath, const std::string &webPath) : 
    configPath(configPath), 
    webPath(webPath)
{}

void DVRLite::Config::Load()
{
    std::ifstream file(configPath);
    if (file.is_open())
    {
        Json::Value config;
        file >> config;

        std::lock_guard lock(configMutex);
        recordPath = config["recordPath"].asString();
    }
}

void DVRLite::Config::Save() const
{
    Json::Value source;
    {
        std::lock_guard lock(configMutex);
        source["recordPath"] = recordPath;
    }
    std::filesystem::path fullpath(configPath);
    std::filesystem::create_directories(fullpath.parent_path());
    std::ofstream file(configPath);
    file << source;
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
