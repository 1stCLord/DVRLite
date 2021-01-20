#pragma once

#include <functional>
#include <thread>
#include <string>
#include <memory>
#include "DVRLite.h"

class PullPointSubscriptionBindingProxy;

class PullPointSubscription
{
public:
	PullPointSubscription(const DVRLite::Source& source, std::function<void(void)> alert);
	~PullPointSubscription();
	PullPointSubscription(const PullPointSubscription&) = delete;
	PullPointSubscription(PullPointSubscription&&) = delete;
	PullPointSubscription& operator=(const PullPointSubscription&) = delete;
	PullPointSubscription& operator=(PullPointSubscription&&) = delete;
private:
	std::atomic_bool running;
	std::thread thread;

	const DVRLite::Source source;
	std::function<void(void)> alert;

	std::string pullpoint;
	std::unique_ptr<PullPointSubscriptionBindingProxy> pullpointSubscriptionBindingProxy;

	void Run();

	std::string Init();
	void PullMessages();
	void Uninit();

};