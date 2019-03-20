#pragma once
#include <memory>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <functional>
#include "AnyValues.h"
namespace fs
{
	namespace L1
	{
		enum class Session_Status : unsigned int
		{
			Normal = 0,
			Running = 1,
			BlackOut = 2,
			Finish = 3
		};
		class Pin;
		class Spore;
		using IdType = unsigned long long;
		class Session : public std::enable_shared_from_this<Session>
		{
		public:
			Session(const std::shared_ptr<Pin>& entryPin);
			Session(const Session&) = delete;
			Session() = delete;
			~Session();
			IdType id();
			void submit(const AnyValues &any = AnyValues());
			void stop();
			void onFinish(std::function<void(std::shared_ptr<Session>)> func);
			void join();
			AnyValues &values() { return _values; }
			Session_Status status() { return _status; }
			std::chrono::high_resolution_clock::time_point startTime() { return _startTime; }
			std::chrono::high_resolution_clock::time_point stopTime() { return _stopTime; }
			static std::shared_ptr<Session> newSession(std::shared_ptr<Pin> entryPin);

		protected:
			void increaseTask();
			void decreaseTask();
		protected:
			IdType _id;
			std::atomic_ullong _task_remain;
			std::function<void(std::shared_ptr<Session>)> _triggerOnFinish;

			std::chrono::high_resolution_clock::time_point _startTime;
			std::chrono::high_resolution_clock::time_point _stopTime;

			AnyValues _values;

			std::shared_ptr<Spore> _entrySpore;
			std::shared_ptr<Pin> _entryPin;
			std::condition_variable  _cond_status;
			mutable std::mutex _mut_status;
			Session_Status _status;
			friend class Pin;
		
		};
	}
}


