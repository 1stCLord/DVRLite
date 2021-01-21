// DVRLite.cpp : Defines the entry point for the application.
//

#include "DVRLite.h"
#include "Web/MediaController.hpp"

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

DVRLite::DVRLite()
{
    //load the sources
    if (std::filesystem::is_directory("sources"))
    {
        for (std::filesystem::directory_entry directory : std::filesystem::directory_iterator("sources"))
        {
            std::filesystem::path configPath = directory.path() / "config.json";
            if (directory.is_directory() && std::filesystem::is_regular_file(configPath))
            {
                Source source(configPath);
                sources.push_back(source);
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
    source.Save(std::string("sources/") + source.GetName() + std::string("/config.json"));
}