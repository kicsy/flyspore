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
	Spore::Spore()
	{
	}

	Spore::~Spore()
	{
	}

	PW_Spore Spore::parent()
	{
		return _parent;
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
		if (pin->_process == nullptr)
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
			pss->increaseTask();
		}
		task_pool::get_instance().submit(&Spore::process, this, pin->_process, data);
		return true;
	}

	std::vector<P_Pin> Spore::pins()
	{
		std::shared_lock<std::shared_mutex> lock(_pins_mutex);
		return _pins;
	}

	P_Pin Spore::addPIn(Pin_Type type, Pin_Process process)
	{
		if (type == Pin_Type::OUT_PIN)
			process = nullptr;
		auto pin = std::make_shared<Pin>(shared_from_this(), type, process);
		std::unique_lock<std::shared_mutex> lock(_pins_mutex);
		_pins.push_back(pin);
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
		walkChilds([&](P_Spore child) ->bool {
			if (child)
			{
				child->buildSession(sessionId);
			}
			return true;
		});
	}

	void Spore::releaseSession(IdType sessionId)
	{

		walkChilds([&](P_Spore child) {
			if (child)
			{
				child->releaseSession(sessionId);
			}
			return true;
		});

		{
			std::unique_lock<std::shared_mutex> lock(_session_mutex);
			if (_sessionValues.count(sessionId))
			{
				_sessionValues[sessionId] = nullptr;
			}
		}
	}

	void Spore::process(Pin_Process pprocess, P_Data data)
	{
		if (pprocess && data)
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
				plocal = _sessionValues[data->getSession()->id()];
			}

			Context cc(getptr(), data->getSession(), plocal);
			pprocess(cc, data);
			pss->decreaseTask();
		}
	}

	P_Path Spore::create_or_find_Path(P_Pin from, P_Pin to)
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
			path = std::make_shared<Path>();
			path->_from = from;
			path->_to = to;
			_paths.push_back(path);
		}
		from->addPath(path);
		return std::move(path);
	}

	P_Spore Spore::addChild(const std::string & name, IdType id)
	{
		P_Spore ps = P_Spore(new Spore(name, id));
		ps->_parent = getptr();
		return std::move(ps);
	}

	P_Spore Spore::newSpore(const std::string &name, IdType id /*= 0*/)
	{
		P_Spore ps = std::make_shared<Spore>(name, id);
		return std::move(ps);
	}

}


