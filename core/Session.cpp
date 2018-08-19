#include <chrono>
#include "Session.h"
#include "task_pool.h"
#include "Spore.h"
fs::Session::Session(const std::string &name, fs::IdType id /*= 0*/):
	Object(name, id)
	,_task_remain(0)
	, _status(Session_Status::Normal)
{
}

fs::Session::~Session()
{
	if (_entrySpore)
	{
		_entrySpore->cleanSession(_this.lock());
	}
}

void fs::Session::submit(const AnyValues &any)
{
	if(_entrySpore)
	{
		P_Pin entryPin = _entrySpore->getPin(_emtryPinName, Pin_Type::IN_PIN);
		if (entryPin)
		{
			entryPin->push(newData(any));
			_startTime = std::chrono::high_resolution_clock::now();
			std::unique_lock<std::mutex> lk(_mut_status);
			_status = Session_Status::Running;
			_cond_status.notify_all();
		}
	}
}

void fs::Session::stop()
{
	std::unique_lock<std::mutex> lk(_mut_status);
	_status = Session_Status::BlackOut;
	_cond_status.notify_all();
}

void fs::Session::onFinish(std::function<void(P_Session)> func)
{
	_triggerOnFinish = func;
}

void fs::Session::join()
{
	std::unique_lock<std::mutex> lk(_mut_status);
	_cond_status.wait(lk, [&] {return _status == Session_Status::Finish; });
}

fs::P_Data fs::Session::newData(const AnyValues &any)
{
	return P_Data(new DataPack(_this.lock(), any));
}

void fs::Session::increaseTask()
{
	_task_remain++;
}

void fs::Session::decreaseTask()
{
	_task_remain--;
	if (_task_remain == 0)
	{
		_stopTime = std::chrono::high_resolution_clock::now();

		if (_triggerOnFinish)
		{
			_triggerOnFinish(_this.lock());
		}

		std::unique_lock<std::mutex> lk(_mut_status);
		_status = Session_Status::Finish;
		_cond_status.notify_all();
	}
}
