#include "Onvif.h"

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>

#include "DVRLite.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"

Onvif::Onvif() : dvrlite(nullptr){}

Onvif::Onvif(DVRLite *dvrlite) : dvrlite(dvrlite)
{
	subscriptions.reserve(dvrlite->GetSources().size());
	std::vector<Source> &sources = dvrlite->GetSources();
	for (Source& source : sources)
	{
		//FFmpeg* ffmpeg = dvrlite->GetFFmpeg(source);
		subscriptions.emplace_back(new PullPointSubscription(source, [&, dvrlite]()
		{
			for (const std::string &trigger : source.GetTriggers())
			{
				//TODO add/remove sources threading
				//TODO out filename
				FFmpeg* ffmpeg = dvrlite->GetFFmpeg(trigger);
				std::string recordPath = dvrlite->GetConfig().GetRecordPath();
				if (!recordPath.empty())
				{
					//std::filesystem::path directory = std::filesystem::path(recordPath) + std::filesystem::path(source.GetName());
					std::filesystem::path directory = recordPath;
					directory.append(source.GetName());
					std::filesystem::create_directories(directory);
					if (ffmpeg)
						ffmpeg->Record(std::chrono::seconds(source.GetDuration()), directory.string() + "/video.mp4");
				}
			}
			std::cout << "alert " << source.GetName() << "\n";
		}));
	}
}
