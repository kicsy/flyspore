#include <algorithm>
#include "Path.h"
#include "Pin.h"
#include "Spore.h"
#include "DefaultNest.h"
namespace fs
{
	namespace L1
	{
		Path::Path(const std::weak_ptr<Spore>& spore, const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string& name) :
			_spore(spore)
			, _from(from)
			, _to(to)
			, _name(name)
		{
		}

		Path::~Path()
		{
		}

		bool Path::move(const std::shared_ptr<Data>& data)
		{
			if (_to)
			{
				_to->push(data);
				return true;
			}
			return false;
		}

		bool Path::attach()
		{
			auto sp = spore();
			if (!sp)
			{
				return false;
			}
			auto nest = sp->nest();
			if (!nest)
			{
				return false;
			}
			auto thisPath = shared_from_this();
			if (_from)
			{
				_from->_outPaths.add(thisPath);
				//_from->_outPaths.push_back(thisPath);
			}
			if (_to)
			{
				_to->_inPaths.add(thisPath);
				//_to->_inPaths.push_back(thisPath);
			}
			return true;
		}

		bool Path::detach()
		{
			auto sp = spore();
			if (!sp)
			{
				return false;
			}
			auto nest = sp->nest();
			if (!nest)
			{
				return false;
			}
			auto thisPath = shared_from_this();
			if (_from)
			{
				auto &paths = _from->_outPaths;
				paths.remove(thisPath);
				//paths.erase(std::remove_if(paths.begin(), paths.end(), [&](const std::shared_ptr<Path>& pp)->bool {
				//	if (pp == thisPath)
				//	{
				//		return true;
				//	}
				//	return false;
				//}), paths.end());
			}
			if (_to)
			{
				auto &paths = _to->_inPaths;
				paths.remove(thisPath);
				//paths.erase(std::remove_if(paths.begin(), paths.end(), [&](const std::shared_ptr<Path>& pp)->bool {
				//	if (pp == thisPath)
				//	{
				//		return true;
				//	}
				//	return false;
				//}), paths.end());
			}
			return true;
		}

		std::shared_ptr<Path> Path::connect(const std::shared_ptr<Pin> &from, const std::shared_ptr<Pin> &to, const std::string &name /*= ""*/)
		{
			if (!from || !to)
			{
				return nullptr;
			}
			auto fromSpore = from->spore();
			auto toSproe = to->spore();
			if (!fromSpore || !toSproe)
			{
				return nullptr;
			}
			if (fromSpore->parent() == toSproe->parent())
			{
				//两个Pin的宿主Spore为同级，通过Path连接
				auto holder = fromSpore->parent();
				if (!holder)
				{
					return nullptr;
				}
				if (from->type() != Pin_Type::OUT_PIN || to->type() != Pin_Type::IN_PIN)
				{
					return nullptr;
				}
				return  holder->addPath(from, to, name);
			}
			else if (fromSpore == toSproe->parent())
			{
				if (from->type() != Pin_Type::IN_PIN || to->type() != Pin_Type::IN_PIN)
				{
					return nullptr;
				}
				return  fromSpore->addPath(from, to, name);
			}
			else if (fromSpore->parent() == toSproe)
			{
				if (from->type() != Pin_Type::OUT_PIN || to->type() != Pin_Type::OUT_PIN)
				{
					return nullptr;
				}
				return  toSproe->addPath(from, to, name);
			}
			return nullptr;
		}

		std::shared_ptr<Path> Path::connect(std::shared_ptr<Spore> &fromSpore, const std::string &fromPinName, std::shared_ptr<Spore> &toSpore, const std::string &toPinName, const std::string &name /*= ""*/)
		{
			if (!fromSpore || !toSpore)
			{
				return nullptr;
			}
			return connect(fromSpore->getPin(fromPinName), toSpore->getPin(toPinName), name);
		}

		bool Path::release(const std::shared_ptr<Path> &path)
		{
			if (path)
			{
				auto holder = path->spore();
				if (holder)
				{
					return holder->removePath(path);
				}
			}
			return false;
		}

		std::shared_ptr<Pin> Path::from() const
		{
			return _from;
		}

		std::shared_ptr<Pin> Path::to() const
		{
			return _to;
		}


		std::shared_ptr<Spore> Path::spore() const
		{
			return _spore.lock();
		}

		std::string Path::name() const
		{
			return _name;
		}

		bool Path::isvalid() const
		{
			return _from != nullptr && _to != nullptr;
		}
	}
}

