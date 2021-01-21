#include "Onvif.h"

#include <iostream>
#include <string>

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
		subscriptions.emplace_back(new PullPointSubscription(source, [&]()
		{
			for (const std::string &trigger : source.GetTriggers())
			{
				//TODO add/remove sources threading
				//TODO out filename
				FFmpeg* ffmpeg = dvrlite->GetFFmpeg(trigger);
				if (ffmpeg)
					ffmpeg->Record(std::chrono::seconds(source.GetDuration()), "video.mp4");
			}
			std::cout << "alert " << source.GetName() << "\n";
		}));
	}
}
