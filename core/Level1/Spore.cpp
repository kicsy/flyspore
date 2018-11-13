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
#include "../Level0/task_pool.h"
#include "Session.h"
#include "Context.h"
#include "DataPack.h"

using namespace fs::L0;

namespace fs
{
	namespace L1
	{
		Spore::Spore(const std::string &name) :
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

		bool Spore::input(const P_Pin &pin, const P_Data &data)
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
			if (!pss)
			{
				return false;
			}
			if (pss->status() == Session_Status::BlackOut)
			{
				return false;
			}
			if (pin->enableProcess())
			{
				pss->increaseTask();
				task_pool::get_instance().submit(&Spore::process, this, pin, data);
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

		P_Pin Spore::getPin(const std::string &name)
		{
			std::shared_lock<std::shared_mutex> lock(_pins_mutex);
			auto iter = _pins.find(name);
			if (iter == _pins.end())
				return nullptr;
			return iter->second;
		}

		P_Pin Spore::addPin(P_Pin &pin)
		{
			if (!pin || pin->spore().get() != this)
			{
				return NULL;
			}
			std::unique_lock<std::shared_mutex> lock(_pins_mutex);
			if (_pins.find(pin->name()) != _pins.end())
				return nullptr;
			_pins[pin->name()] = pin;
			return pin;
		}

		bool Spore::deletePin(P_Pin &pin)
		{
			if (!pin || pin->spore().get() != this)
			{
				return false;
			}
			{
				std::unique_lock<std::shared_mutex> lock(_pins_mutex);
				auto iter = _pins.find(pin->name());
				if (iter == _pins.end())
				{
					return false;
				}
				pin = iter->second;
				_pins.erase(iter);
			}
			if (pin)
			{
				for (const P_Path& path : pin->_inPaths)
				{
					Path::release(path);
				}
				for (const P_Path& path : pin->_outPaths)
				{
					Path::release(path);
				}
			}
			return true;
		}

		bool Spore::deletePin(const std::string &name)
		{
			return deletePin(getPin(name));
		}

		std::vector<P_Spore> Spore::childs()
		{
			std::shared_lock<std::shared_mutex> lock(_childs_mutex);
			return _childs;
		}

		P_Spore Spore::getChild(const std::string &name)
		{
			std::shared_lock<std::shared_mutex> lock(_childs_mutex);
			auto iter = std::find_if(_childs.begin(), _childs.end(), [&](P_Spore& pp) {
				return pp->name() == name;
			});
			if (iter != _childs.end())
			{
				return *iter;
			}
			return NULL;
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

		P_Spore Spore::addChild(const std::string &name)
		{
			return addChild(newSpore(name));
		}

		P_Spore Spore::removeChild(P_Spore child)
		{
			if (!child)
			{
				return nullptr;
			}
			return removeChild(child->name());
		}

		P_Spore Spore::removeChild(const std::string &name)
		{
			std::unique_lock<std::shared_mutex> lock(_childs_mutex);
			auto iter = std::remove_if(_childs.begin(), _childs.end(), [&](P_Spore& pp) {
				return pp->name() == name;
			});
			P_Spore theRemove = *iter;
			if (!theRemove)
			{
				return nullptr;
			}
			for (const P_Path& path : _paths)
			{
				if ((path->from() && path->from()->spore() == theRemove) ||
					(path->to() && path->to()->spore() == theRemove))
				{
					Path::release(path);
				}
			}
			theRemove->cleanAllSession();
			theRemove->_parent = nullptr;
			return theRemove;
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

		void Spore::cleanAllSession()
		{
			{
				std::shared_lock<std::shared_mutex> lock(_childs_mutex);
				for (auto child : _childs)
				{
					child->cleanAllSession();
				}
			}
			{
				std::unique_lock<std::shared_mutex> lock(_session_mutex);
				_sessionValues.clear();
			}
		}

		void Spore::process(const P_Pin &pin, const P_Data &data)
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
				pin->process(cc, data);
				pss->decreaseTask();
			}
		}

		P_Path Spore::create_or_find_Path(P_Pin from, P_Pin to, const std::string &name /*= ""*/)
		{
			std::unique_lock<std::shared_mutex> lock(_paths_mutex);
			auto iter = std::find_if(_paths.begin(), _paths.end(), [&](P_Path& pp) {
				return pp->from() == from && pp->to() == to;
			});
			if (iter != _paths.end())
			{
				return *iter;
			}
			P_Path path = std::make_shared<Path>(from, to, name);
			path->_build();
			_paths.push_back(path);
			return std::move(path);
		}

		bool Spore::deletePath(const P_Path &path)
		{
			bool isOk = false;
			std::unique_lock<std::shared_mutex> lock(_paths_mutex);
			isOk = std::remove_if(_paths.begin(), _paths.end(), [&](P_Path& pp)->bool {
				if (pp == path)
				{
					pp->_release();
					return true;
				}
			}) != _paths.end();
			return isOk;
		}

		P_Spore Spore::newSpore(const std::string &name)
		{
			P_Spore ps = std::make_shared<Spore>(name);
			return std::move(ps);
		}
	}
}

