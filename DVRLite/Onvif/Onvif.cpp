#include "Onvif.h"

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>

#include "DVRLite.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"
#include "PullPointSubscription.h"
#include "Web/Utils.hpp"

Onvif::Onvif() : dvrlite(nullptr){}

Onvif::Onvif(DVRLite *dvrlite)
{
	Init(dvrlite);
}

void Onvif::Init(DVRLite *dvrlite)
{
	this->dvrlite = dvrlite;
	subscriptions.clear();
	subscriptions.reserve(dvrlite->GetSources().size());
	DVRLite::SourceSet& sources = dvrlite->GetSources();
	for (const Source& source : sources)
	{
		Add(source);
	}
}

void Onvif::Add(const Source &source)
{
	DVRLite::Log("Onvif Adding " + source.GetName());
	subscriptions.emplace_back(new PullPointSubscription(const_cast<Source&>(source), [source, this]()
	{
		DVRLite::Log("Alert " + source.GetName());
		for (const std::string& trigger : source.GetTriggers())
		{
			FFmpeg* ffmpeg = dvrlite->GetFFmpeg(trigger);
			std::string recordPath = dvrlite->GetConfig().GetRecordPath();
			if (!recordPath.empty())
			{
				std::filesystem::path directory = recordPath;
				directory.append(source.GetName());
				std::filesystem::create_directories(directory);
				std::string filename = to_string(std::chrono::system_clock::now(), std::string(DATEFILEFORMAT) + ".mp4");
				if (ffmpeg)
					ffmpeg->Record(std::chrono::seconds(source.GetDuration()), std::filesystem::path(directory.string()) / filename);
				else
					DVRLite::Log("No FFmpeg instance found " + source.GetName());
			}
			else
				DVRLite::Log("Record Path Empty");
		}
	}));
	subscriptions.back()->call();
}
void Onvif::Remove(const std::string &sourceName)
{
	DVRLite::Log("Onvif Removing " + sourceName);
	std::vector<std::unique_ptr<PullPointSubscription>>::iterator it = std::find_if(subscriptions.begin(), subscriptions.end(),
		[&](std::unique_ptr<PullPointSubscription>& pullpoint) 
		{
			return pullpoint.get()->source.GetName() == sourceName; 
		});
	it->reset();
	subscriptions.erase(it);
}