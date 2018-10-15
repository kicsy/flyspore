#include <chrono>
#include "Session.h"
#include "task_pool.h"
#include "Spore.h"
fs::Session::Session(fs::P_Pin entryPin, const std::string &name, fs::IdType id /*= 0*/) :
	Object(name, id)
	,_task_remain(0)
	, _status(Session_Status::Normal)
{
	if (entryPin && entryPin->type() == fs::Pin_Type::IN_PIN)
	{
		_entryPin = entryPin;
		_entrySpore = entryPin->parent();
		if (_entrySpore)
		{
			_entrySpore->buildSession(_id);
		}
	}
}

fs::Session::~Session()
{
	if (_entrySpore)
	{
		_entrySpore->releaseSession(id());
	}
}

void fs::Session::submit(const AnyValues &any)
{
	if(_entrySpore && _entryPin)
	{
		_entryPin->push(newData(any));
		_startTime = std::chrono::high_resolution_clock::now();
		std::unique_lock<std::mutex> lk(_mut_status);
		_status = Session_Status::Running;
		_cond_status.notify_all();
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
	return P_Data(new DataPack(getptr(), any));
}

fs::P_Session fs::Session::newSession(P_Pin entryPin, const std::string & name, IdType id)
{
	return std::make_shared<fs::Session>(entryPin, name, id);
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
			_triggerOnFinish(getptr());
		}

		std::unique_lock<std::mutex> lk(_mut_status);
		_status = Session_Status::Finish;
		_cond_status.notify_all();
	}
}
