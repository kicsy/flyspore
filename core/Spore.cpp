#include "Spore.h"
#include "Pin.h"
#include "task_pool.h"
#include "Session.h"
#include "Context.h"

fs::Spore::Spore(const std::string  &name, IdType id /*= 0*/)
	:Object<Spore, Spore>(name, id)
{

}

bool fs::Spore::input(P_Pin pin, C_Data data)
{
	if (pin == nullptr )
	{
		return false;
	}
	auto pinParent = pin->parent();
	auto thisSpore = _this.lock();
	if (pinParent != thisSpore || (unsigned int)pin->_indexInSpore >= _pins.size())
	{
		return false;
	}
	Pin_Process &pc = _handler[pin->_indexInSpore];
	if (pc == nullptr)
	{
		return false;
	}
	//task_pool::get_instance().submit(pc, data);
	P_Session pss = data->getSession();
	if (pss)
	{
		if (pss->status() == Session_Status::BlackOut)
		{
			return false;
		}
		pss->increaseTask();
	}
	task_pool::get_instance().submit(&fs::Spore::process, this, pc, data);
	return true;
}

std::vector<fs::P_Pin> fs::Spore::pins()
{
	return _pins;
}

fs::P_Pin fs::Spore::getPin(const std::string &name, Pin_Type type)
{
	auto iter = std::find_if(_pins.begin(), _pins.end(), [name, type](P_Pin& pp){
		if (pp && pp->name() == name)
		{
			return (type == Pin_Type::UNKNOW || type == pp->type());
		}
		return true;
	});
	if (iter != _pins.end())
	{
		return *iter;
	}
	return nullptr;
}

fs::P_Pin fs::Spore::addPIn(const std::string &pinName, Pin_Type type, Pin_Process process)
{
	auto pin = std::make_shared<Pin>(pinName);// std::shared_ptr<Pin>(new Pin(name));
	pin->_parent = _this;
	pin->_this = pin;
	pin->_type = type;
	pin->_indexInSpore = (int)_pins.size();
	_pins.push_back(pin);
	_handler.push_back(process);
	return pin;
	//return std::move(pin);
}

fs::P_Pin fs::Spore::addInPIn(const std::string & pinName, Pin_Process process)
{
	return addPIn(pinName, Pin_Type::IN_PIN, process);
}

fs::P_Pin fs::Spore::addOutPIn(const std::string & pinName)
{
	return addPIn(pinName, Pin_Type::OUT_PIN);
}

void fs::Spore::buildSession(IdType sessionId)
{
	{
		std::unique_lock<std::shared_mutex> lock(_shared_mutex_session);
		if (_sessionValues.count(sessionId))
		{
			_sessionValues[sessionId] = nullptr;
		}
		_sessionValues[sessionId] = std::make_shared<AnyValues>();
	}
	for (auto child : _childs)
	{
		child->buildSession(sessionId);
	}
}

void fs::Spore::releaseSession(IdType sessionId)
{
	{
		std::unique_lock<std::shared_mutex> lock(_shared_mutex_session);
		if (_sessionValues.count(sessionId))
		{
			_sessionValues[sessionId] = nullptr;
		}
	}
	for (auto child : _childs)
	{
		child->releaseSession(sessionId);
	}
}

void fs::Spore::process(Pin_Process pprocess, C_Data data)
{
	if (pprocess && data )
	{
		P_Session pss = data->getSession();
		if (pss == nullptr)
		{
			return;
		}

		if (pss->status() == Session_Status::BlackOut)
		{
			pss->decreaseTask();
			return;
		}

		P_AnyValues plocal;
		{
			std::shared_lock<std::shared_mutex> lock(_shared_mutex_session);
			plocal = _sessionValues[data->getSession()->id()];
		}
		Context cc(_this.lock(), data->getSession(), plocal);
		pprocess(cc, data);
		pss->decreaseTask();
	}
}

fs::P_Spore fs::Spore::addChild(const std::string & name, IdType id)
{
	P_Spore ps = P_Spore(new Spore(name, id));
	ps->_parent = _this;
	ps->_this = ps;
	return std::move(ps);
}

fs::P_Session fs::Spore::newSession(const std::string &entryPinName, const std::string &name)
{

	auto pss = P_Session(new Session(name));
	pss->_entrySpore = _this.lock();
	pss->_emtryPinName = entryPinName;
	pss->_this = pss;
	buildSession(pss->id());
	for (auto ps : _childs)
	{
		if (ps)
		{
			ps->buildSession(pss->id());
		}
	}
	return pss;
}

void fs::Spore::cleanSession(P_Session pss)
{
	if (!pss)
	{
		return;
	}
	for (auto ps : _childs)
	{
		if (ps)
		{
			ps->releaseSession(pss->id());
		}
	}
	releaseSession(pss->id());
}

fs::P_Spore fs::Spore::newSpore(const std::string &name, IdType id /*= 0*/)
{
	P_Spore ps = std::make_shared<Spore>(name, id);
	ps->_this = ps;
	return std::move(ps);
}


