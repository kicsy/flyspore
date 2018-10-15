
#pragma once
#include <memory>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include "Object.h"
namespace fs
{
	class Spore;
	class Session;
	class Pin;
	using P_Session = std::shared_ptr<Session>;
	using P_Spore = std::shared_ptr<Spore>;
	using P_Pin = std::shared_ptr<Pin>;
	enum class Session_Status : unsigned int 
	{
		Normal = 0,
		Running = 1,
		BlackOut = 2,
		Finish = 3
	};

	class Session : public Object<Session, Session>
	{
	public:
		Session(P_Pin entryPin, const std::string  &name, IdType id = 0);
		Session(const Session&) = delete;
		Session() = delete;
		~Session();
		void submit(const AnyValues &any = AnyValues());
		void stop();
		void onFinish(std::function<void(P_Session)> func);
		void join();
		Session_Status status() { return _status; }
		std::chrono::high_resolution_clock::time_point startTime() { return _startTime; }
		std::chrono::high_resolution_clock::time_point stopTime() { return _stopTime; }
		P_Data newData(const AnyValues &any = AnyValues());
		static P_Session newSession(P_Pin entryPin, const std::string  &name = "", IdType id = 0);

	protected:
		std::shared_ptr<Traceable> parent() = delete;
		std::vector<std::shared_ptr<Traceable>> childs() = delete;
		void increaseTask();
		void decreaseTask();
	protected:
		std::atomic_ullong _task_remain;
		std::function<void(P_Session)> _triggerOnFinish;

		
		std::chrono::high_resolution_clock::time_point _startTime;
		std::chrono::high_resolution_clock::time_point _stopTime;

		P_Spore _entrySpore;
		std::string _emtryPinName;
		P_Pin _entryPin;
		std::condition_variable  _cond_status;
		mutable std::mutex _mut_status;
		Session_Status _status;
		friend class Spore;

	};
}


