/*!
 * \class classname
 *
 * \ingroup GroupName
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note 
 *
 * \author kicsy
 *
 * \version 1.0
 *
 * \date 2016/08/19 12:38
 *
 * Contact: lmj07luck@126.com
 *
 */
#include "Spore.h"
#include "Pin.h"
#include "Path.h"
#include "task_pool.h"
#include "Session.h"
#include "Context.h"
#include "DataPack.h"
namespace fs
{
	Spore::Spore(const std::string &name):
		_name(name)
	{
	}

	Spore::~Spore()
	{
	}

	PW_Spore Spore::parent()
	{
		return _parent;
	}

	std::string Spore::name()
	{
		return _name;
	}

	bool Spore::input(P_Pin pin, P_Data data)
	{
		if (pin == nullptr)
		{
			return false;
		}
		auto inSpore = pin->spore();
		if (inSpore != shared_from_this())
		{
			return false;
		}
		P_Session pss = data->getSession();
		if (pss)
		{
			if (pss->status() == Session_Status::BlackOut)
			{
				return false;
			}
		}
		if (pin->_process)
		{
			pss->increaseTask();
			task_pool::get_instance().submit(&Spore::process, this, pin->_process, data);
		}
		return true;
	}

	std::vector<P_Pin> Spore::pins()
	{
		std::vector<P_Pin> ps(_pins.size());
		std::shared_lock<std::shared_mutex> lock(_pins_mutex);
		for (const auto& pin : _pins)
		{
			ps.push_back(pin.second);
		}
		return std::move(ps);
	}

	fs::P_Pin Spore::getPin(const std::string &name)
	{
		std::shared_lock<std::shared_mutex> lock(_pins_mutex);
		auto iter = _pins.find(name);
		if (iter  == _pins.end())
			return nullptr;
		return iter->second;
	}

	P_Pin Spore::addPin(const std::string &name, Pin_Process process)
	{
		return addPin(name, process == nullptr ? Pin_Type::OUT_PIN : Pin_Type::IN_PIN, process);
	}

	fs::P_Pin Spore::addPin(const std::string &name, Pin_Type type, Pin_Process process)
	{
		auto pin = P_Pin(new Pin(weak_from_this(), name, type, process));
		std::unique_lock<std::shared_mutex> lock(_pins_mutex);
		if (_pins.find(name) != _pins.end())
			return nullptr;
		_pins[name] = pin;
		return pin;
	}

	std::vector<P_Spore> Spore::childs()
	{
		std::shared_lock<std::shared_mutex> lock(_childs_mutex);
		return _childs;
	}

	P_Spore Spore::addChild(P_Spore child)
	{
		if (!child)
			return nullptr;
		child->_parent = shared_from_this();
		std::unique_lock<std::shared_mutex> lock(_childs_mutex);
		_childs.push_back(child);
		return child;
	}

	fs::P_Spore Spore::addChild(const std::string &name)
	{
		return addChild(newSpore(name));
	}

	void Spore::buildSession(IdType sessionId)
	{
		{
			std::unique_lock<std::shared_mutex> lock(_session_mutex);
			if (!_sessionValues.count(sessionId))
			{
				_sessionValues[sessionId] = std::make_shared<AnyValues>();
			}
		}
		{
			std::shared_lock<std::shared_mutex> lock(_childs_mutex);
			for (auto child : _childs)
			{
				child->buildSession(sessionId);
			}
		}
	}

	void Spore::releaseSession(IdType sessionId)
	{
		{
			std::shared_lock<std::shared_mutex> lock(_childs_mutex);
			for (auto child : _childs)
			{
				child->releaseSession(sessionId);
			}
		}
		{
			std::unique_lock<std::shared_mutex> lock(_session_mutex);
			if (_sessionValues.count(sessionId))
			{
				_sessionValues[sessionId] = nullptr;
			}
		}
	}

	void Spore::process(P_Pin pin, P_Data data)
	{
		if (pin && data)
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
				std::shared_lock<std::shared_mutex> lock(_session_mutex);
				plocal = _sessionValues[pss->id()];
			}
			Context cc(shared_from_this(), pss, plocal);
			pin->process(&cc, data);
			pss->decreaseTask();
		}
	}

	P_Path Spore::create_or_find_Path(P_Pin from, P_Pin to, const std::string &name /*= ""*/)
	{
		P_Path path;
		{
			std::unique_lock<std::shared_mutex> lock(_paths_mutex);
			auto iter = std::find_if(_paths.begin(), _paths.end(), [&](P_Path& pp) {
				return pp->from() == from && pp->to() == to;
			});
			if (iter != _paths.end())
			{
				return *iter;
			}
			path = P_Path(new Path(name));
			path->_from = from;
			path->_to = to;
			_paths.push_back(path);
		}
		from->addPath(path);
		return std::move(path);
	}


	P_Spore Spore::newSpore(const std::string &name)
	{
		P_Spore ps = std::make_shared<Spore>(name);
		return std::move(ps);
	}

}


