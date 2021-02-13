#pragma once

#include <functional>
#include <thread>
#include <string>
#include <memory>
#include "Source.h"
#include "Log.h"

struct soap;
class PullPointSubscriptionBindingProxy;
namespace DVRLite
{
	class PullPointSubscription
	{
	public:
		PullPointSubscription(Source& source, std::function<void(const std::string&)> alert);
		~PullPointSubscription();
		PullPointSubscription(const PullPointSubscription&) = delete;
		PullPointSubscription(PullPointSubscription&&) = delete;
		PullPointSubscription& operator=(const PullPointSubscription&) = delete;
		PullPointSubscription& operator=(PullPointSubscription&&) = delete;

		void call(const std::string& message) { alert(message); }

		Source& source;
	private:
		std::atomic_bool running;
		std::thread thread;

		std::function<void(const std::string&)> alert;

		std::string pullpoint;
		std::unique_ptr<PullPointSubscriptionBindingProxy> pullpointSubscriptionBindingProxy;

		static const Logger::LogFilter filter = Logger::LogFilter::PullPointSubscription;

		void Run();

		std::string Init();
		bool PullMessages();
		void Renew();
		void Uninit();

		static std::string ManualParseMessage(const std::string& buffer);
		static void SetTimeouts(soap* soap);

	};
}