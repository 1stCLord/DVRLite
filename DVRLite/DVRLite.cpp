// DVRLite.cpp : Defines the entry point for the application.
//

#include "DVRLite.h"
#include "Web/MediaController.hpp"
#include "Onvif/Onvif.h"
#include "FFmpeg/FFmpeg.h"

#include <fstream>

int main(int argc, const char* argv[]) 
{
    using namespace std::chrono_literals;
    FFmpeg ffmpeg = FFmpeg("rtmp://192.168.0.75/bcs/channel0_main.bcs?channel=0&stream=0&user=admin&password=gibraltar", false, true);
    ffmpeg.Record(10s, "video.mp4");
    std::this_thread::sleep_for(8s);
    ffmpeg.Record(10s, "video.mp4");
    std::this_thread::sleep_for(8s);
    ffmpeg.Record(14s, "video.mp4");

    /*while(ffmpeg.IsRecording())
        std::this_thread::sleep_for(1s);*/
    /*DVRLite dvrlite;
    Onvif onvif(&dvrlite);
    MediaController::run(&dvrlite);*/

    return 0;
}

DVRLite::Source::Source(const QueryParams &queryParams)
{
    using String = oatpp::data::mapping::type::String;
    using StringKeyLabel = oatpp::data::share::StringKeyLabel;
    using MatchMapPair = std::pair<StringKeyLabel, StringKeyLabel>;

    Json::Value source;
    name = queryParams.get("name")->std_str();
    address = queryParams.get("address")->std_str();
    address = createFullOnvifPath(unescapeUrl(address));

    username = queryParams.get("username")->std_str();
    password = queryParams.get("password")->std_str();

    std::string variable = std::string("self_trigger");
    String value = queryParams.get(variable.c_str());
    if (value && value->std_str() == "on")
        triggers.push_back(name);
    for (const MatchMapPair& queryParam : queryParams.getAll())
    {
        std::string start = "trigger_";
        std::string query = queryParam.first.std_str();
        std::string value = queryParam.second.std_str();
        if (query.substr(0, start.size()) == start)
        {
            if (value == "on")
                triggers.push_back(query.substr(start.size()));
        }
    }

    duration = std::stoi(queryParams.get("duration")->std_str());
    quota = std::stoi(queryParams.get("quota")->std_str());
}

void DVRLite::Source::Save(const std::filesystem::path &path) const
{
    Json::Value source;
    source["name"] = name;
    source["address"] = address;
    source["username"] = username;
    source["password"] = password;
    source["triggers"] = Json::Value(Json::arrayValue);
    for(std::string trigger : triggers)
        source["triggers"].append(trigger);
    source["duration"] = duration;
    source["quota"] = quota;
    
    std::filesystem::path fullpath(path);
    std::filesystem::create_directories(fullpath.parent_path());
    std::ofstream file(path);
    file << source;
}

void DVRLite::Source::Load(const std::filesystem::path &path)
{
    std::ifstream file(path);
    Json::Value source;
    file >> source;
    name = source["name"].asString();
    address = source["address"].asString();
    username = source["username"].asString();
    password = source["password"].asString();
    for (Json::Value& triggerValue : source["triggers"])
        triggers.push_back(triggerValue.asString());
    duration = source["duration"].asUInt();
    quota = source["quota"].asUInt();
}

DVRLite::DVRLite()
{
    if (std::filesystem::is_directory("sources"))
    {
        for (std::filesystem::directory_entry directory : std::filesystem::directory_iterator("sources"))
        {
            std::filesystem::path configPath = directory.path() / "config.json";
            if (directory.is_directory() && std::filesystem::is_regular_file(configPath))
            {
                sources.push_back(Source(configPath));
            }
        }
    }
    //todo initialise onvif
}

void DVRLite::AddSource(const Source &source)
{
    sources.push_back(source);
    source.Save(std::string("sources/") + source.name + std::string("/config.json"));
}