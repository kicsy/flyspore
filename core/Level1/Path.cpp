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
			_name(name)
			, _from(from)
			, _to(to)
			, _spore(spore)
			,_pvalues(nullptr)
		{
		}

		Path::~Path()
		{
			if(_pvalues)
			{
				delete _pvalues;
				_pvalues = nullptr;
			}
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
				_from->_outPaths().add(thisPath);
			}
			if (_to)
			{
				_to->_inPaths().add(thisPath);
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
				_from->_outPaths().remove(thisPath);
			}
			if (_to)
			{
				_to->_inPaths().remove(thisPath);
			}
			return true;
		}

		std::shared_ptr<Path> Path::connect(const std::shared_ptr<Pin> &from, const std::shared_ptr<Pin> &to, const std::string &name /*= ""*/)
		{
			if (!from || !to || from->nest() != to->nest())
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
				//����Pin������SporeΪͬ����ͨ��Path����
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

		AnyValues& Path::propertys()
		{
			if(!isvalid())
			{
				return _propertys();
			}
			NestUniqueLock lock(_from->nest());
			return _propertys();
		}
				
		inline AnyValues& Path::_propertys()
		{
			if(!_pvalues)
			{
				_pvalues = new AnyValues();
			}
			return *_pvalues;
		}
	}
}

