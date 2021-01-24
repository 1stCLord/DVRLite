#include "Onvif.h"

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>

#include "DVRLite.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"
#include "PullPointSubscription.h"

Onvif::Onvif() : dvrlite(nullptr){}

Onvif::Onvif(DVRLite *dvrlite) : dvrlite(dvrlite)
{
	subscriptions.reserve(dvrlite->GetSources().size());
	DVRLite::SourceSet &sources = dvrlite->GetSources();
	for (const Source& source : sources)
	{
		Add(source);
	}
}

void Onvif::Add(const Source &source)
{
	//FFmpeg* ffmpeg = dvrlite->GetFFmpeg(source);
	subscriptions.emplace_back(new PullPointSubscription(const_cast<Source&>(source), [&]()
	{
		for (const std::string& trigger : source.GetTriggers())
		{
			//TODO out filename
			FFmpeg* ffmpeg = dvrlite->GetFFmpeg(trigger);
			std::string recordPath = dvrlite->GetConfig().GetRecordPath();
			if (!recordPath.empty())
			{
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
void Onvif::Remove(const std::string &sourceName)
{
	std::vector<std::unique_ptr<PullPointSubscription>>::iterator it = std::find_if(subscriptions.begin(), subscriptions.end(),
		[&](std::unique_ptr<PullPointSubscription>& pullpoint) 
		{
			return pullpoint.get()->source.GetName() == sourceName; 
		});
	it->reset();
	subscriptions.erase(it);

	/*for (std::unique_ptr<PullPointSubscription> &subscription : subscriptions)
	{
		if (source == subscription.get()->source.GetName())
			subscription.reset();
	}*/
}