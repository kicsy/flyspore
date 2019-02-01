#pragma once
#include <memory>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include "Statement.h"
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

		class Session : public std::enable_shared_from_this<Session>
		{
		public:
			Session(P_Pin entryPin);
			Session(const Session&) = delete;
			Session() = delete;
			~Session();
			IdType id();
			void submit(const AnyValues &any = AnyValues());
			void stop();
			void onFinish(std::function<void(P_Session)> func);
			void join();
			AnyValues &values() { return _values; }
			Session_Status status() { return _status; }
			std::chrono::high_resolution_clock::time_point startTime() { return _startTime; }
			std::chrono::high_resolution_clock::time_point stopTime() { return _stopTime; }
			static P_Session newSession(P_Pin entryPin);

		protected:
			void increaseTask();
			void decreaseTask();
		protected:
			IdType _id;
			std::atomic_ullong _task_remain;
			std::function<void(P_Session)> _triggerOnFinish;

			std::chrono::high_resolution_clock::time_point _startTime;
			std::chrono::high_resolution_clock::time_point _stopTime;

			AnyValues _values;

			P_Spore _entrySpore;
			P_Pin _entryPin;
			std::condition_variable  _cond_status;
			mutable std::mutex _mut_status;
			Session_Status _status;
			friend class Pin;
		
		};
	}
}


