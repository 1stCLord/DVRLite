#include "Onvif.h"

#include <iostream>
#include <string>

#include "DVRLite.h"

Onvif::Onvif(DVRLite const *dvrlite) : dvrlite(dvrlite)
{
	subscriptions.reserve(dvrlite->GetSources().size());
	for (const DVRLite::Source& source : dvrlite->GetSources())
	{
		subscriptions.emplace_back(new PullPointSubscription(source, [&]()
		{
			std::cout << "alert " << source.name << "\n";
		}));
	}
}
