#pragma once

#include <functional>
#include <thread>
#include <string>
#include <memory>
#include "Source.h"

class PullPointSubscriptionBindingProxy;

class PullPointSubscription
{
public:
	PullPointSubscription(Source& source, std::function<void(void)> alert);
	~PullPointSubscription();
	PullPointSubscription(const PullPointSubscription&) = delete;
	PullPointSubscription(PullPointSubscription&&) = delete;
	PullPointSubscription& operator=(const PullPointSubscription&) = delete;
	PullPointSubscription& operator=(PullPointSubscription&&) = delete;

	void call() { alert(); }

	Source& source;
private:
	std::atomic_bool running;
	std::thread thread;

	std::function<void(void)> alert;

	std::string pullpoint;
	std::unique_ptr<PullPointSubscriptionBindingProxy> pullpointSubscriptionBindingProxy;

	void Run();

	std::string Init();
	void PullMessages();
	void Uninit();

};