#include <chrono>
#include "Session.h"
#include "Spore.h"
#include "DataPack.h"
namespace fs
{
	namespace L1
	{
		Session::Session(P_Pin entryPin) :
			_task_remain(0)
			, _status(Session_Status::Normal)
		{
			auto time_now = std::chrono::system_clock::now();
			auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
			_id = duration_in_ms.count();

			if (entryPin && entryPin->type() == Pin_Type::IN_PIN)
			{
				_entryPin = entryPin;
				_entrySpore = entryPin->spore();
				if (_entrySpore)
				{
					_entrySpore->buildSession(_id);
				}
			}
		}

		Session::~Session()
		{
			if (_entrySpore)
			{
				_entrySpore->releaseSession(_id);
			}
		}

		IdType Session::id()
		{
			return _id;
		}

		void Session::submit(const AnyValues &any)
		{
			if (_entrySpore && _entryPin && _entryPin->adapter())
			{

				P_Data pdata = _entryPin->adapter()->toData(any);
				pdata->setSession(shared_from_this());
				_entryPin->push(pdata);
				_startTime = std::chrono::high_resolution_clock::now();
				std::unique_lock<std::mutex> lk(_mut_status);
				_status = Session_Status::Running;
				_cond_status.notify_all();
			}
		}

		void Session::stop()
		{
			std::unique_lock<std::mutex> lk(_mut_status);
			_status = Session_Status::BlackOut;
			_cond_status.notify_all();
		}

		void Session::onFinish(std::function<void(P_Session)> func)
		{
			_triggerOnFinish = func;
		}

		void Session::join()
		{
			std::unique_lock<std::mutex> lk(_mut_status);
			_cond_status.wait(lk, [&] {return _status == Session_Status::Finish; });
		}

		P_Session Session::newSession(P_Pin entryPin)
		{
			return std::make_shared<Session>(entryPin);
		}

		void Session::increaseTask()
		{
			_task_remain.fetch_add(1);
		}

		void Session::decreaseTask()
		{
			_task_remain.fetch_sub(1);
			if (_task_remain.load(std::memory_order_relaxed) == 0)
			{
				_stopTime = std::chrono::high_resolution_clock::now();
				if (_triggerOnFinish)
				{
					_triggerOnFinish(shared_from_this());
				}

				std::unique_lock<std::mutex> lk(_mut_status);
				_status = Session_Status::Finish;
				_cond_status.notify_all();
			}
		}

	}
}
