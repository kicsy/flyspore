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
#include <algorithm>
#include "Spore.h"
#include "Pin.h"
#include "Path.h"
#include "Session.h"
#include "Context.h"
#include "DataPack.h"
#include "DefaultNest.h"

namespace fs
{
	namespace L1
	{
		Spore::Spore(const std::weak_ptr<DefaultNest>& pNest, const std::string& name):
		_name(name)
		,_nest(pNest)
		{
		}

		Spore::~Spore()
		{
		}

		std::shared_ptr<Spore> Spore::addSpore(const std::shared_ptr<Spore>& spore)
		{
			if (!spore || spore->parent() || !nest())
			{
				return nullptr;
			}
			NestUniqueLock lock(_nest);
			if (std::find(_childs.begin(), _childs.end(), spore) != _childs.end())
			{
				return spore;
			}
			spore->_parent = weak_from_this();
			_childs.push_back(spore);
			nest()->onAddSpore(shared_from_this(), spore);
			return spore;
		}

		std::shared_ptr<Spore> Spore::remvoeSpore(const std::shared_ptr<Spore>& spore)
		{
			if (!spore || spore->parent() != shared_from_this() || !nest())
			{
				return nullptr;
			}

			NestUniqueLock lock(_nest);
			auto iter = std::find(_childs.begin(), _childs.end(), spore);
			if(iter == _childs.end())
			{
				return nullptr;
			}
			spore->_parent.reset();
			_childs.erase(iter);
			nest()->onRemoveSpore(shared_from_this(), spore);
			return spore;
		}

		std::shared_ptr<Spore> Spore::getSpore(const std::string& name)
		{
			if (name.length() == 0 || !nest())
			{
				return nullptr;
			}
			NestUniqueLock lock(_nest);
			auto iter = std::find_if(_childs.begin(), _childs.end(), [&](const std::shared_ptr<Spore>& spore) {
				return spore && spore->name() == name;
			});
			if (iter != _childs.end())
			{
				return *iter;
			}
			return nullptr;
		}

		std::shared_ptr<Pin> Spore::addPin(const std::shared_ptr<Pin>& pin)
		{
			if (!pin || pin->spore() || !nest())
			{
				return nullptr;
			}
			NestUniqueLock lock(_nest);
			if (std::find(_pins.begin(), _pins.end(), pin) != _pins.end())
			{
				return pin;
			}
			pin->_spore = weak_from_this();
			_pins.push_back(pin);
			nest()->onAddPin(shared_from_this(), pin);
			return pin;
		}

		std::shared_ptr<Pin> Spore::removePin(const std::shared_ptr<Pin>& pin)
		{
			if (!pin || pin->spore() != shared_from_this() || !nest())
			{
				return nullptr;
			}

			NestUniqueLock lock(_nest);
			auto iter = std::find(_pins.begin(), _pins.end(), pin);
			if (iter == _pins.end())
			{
				return nullptr;
			}
			pin->_spore.reset();
			_pins.erase(iter);
			nest()->onRemovePin(shared_from_this(), pin);
			return pin;
		}

		std::shared_ptr<Pin> Spore::getPin(const std::string& name) const
		{
			if (name.length() == 0 || !nest())
			{
				return nullptr;
			}
			NestSharedLock lock(_nest);
			auto iter = std::find_if(_pins.begin(), _pins.end(), [&](const std::shared_ptr<Pin>& pin) {
				return pin && pin->name() == name;
			});
			if (iter != _pins.end())
			{
				return *iter;
			}
			return nullptr;
		}

		std::string Spore::name() const
		{
			return _name;
		}

		std::shared_ptr<Spore> Spore::parent() const
		{
			return _parent.lock();
		}

		std::shared_ptr<DefaultNest> Spore::nest() const
		{
			return _nest.lock();
		}

		const std::shared_ptr<Path> Spore::addPath(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string &name /*= ""*/)
		{
			if (!from || !to ||  !nest())
			{
				return nullptr;
			}
			NestUniqueLock lock(_nest);
			auto iter = std::find_if(_paths.begin(), _paths.end(), [&](const std::shared_ptr<Path>& pp) {
				return pp->from() == from && pp->to() == to;
			});
			if (iter != _paths.end())
			{
				return *iter;
			}
			std::shared_ptr<Path> path(new Path(shared_from_this(), from, to, name));
			path->attach();
			_paths.push_back(path);
			nest()->onAddPath(shared_from_this(), path);
			return path;
		}

		bool Spore::removePath(const std::shared_ptr<Path>& path)
		{
			bool isOk = false;
			NestUniqueLock lock(_nest);
			isOk = std::remove_if(_paths.begin(), _paths.end(), [&](const std::shared_ptr<Path>& pp)->bool {
				if (pp == path)
				{
					pp->detach();
					return true;
				}
				return false;
			}) != _paths.end();
			return isOk;
		}
	}
}


