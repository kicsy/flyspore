#include <algorithm>
#include "Path.h"
#include "Pin.h"
#include "Spore.h"
namespace fs
{
	namespace L1
	{
		Path::Path(P_Pin from, P_Pin to, const std::string &name /*= ""*/) :
			std::enable_shared_from_this<Path>()
			, _name(name)
			, _from(from)
			, _to(to)
		{
		}

		Path::~Path()
		{

		}

		bool Path::move(P_Data data)
		{
			if (_to)
			{
				_to->push(data);
				return true;
			}
			return false;
		}

		bool Path::_build()
		{
			return _from && _from->addPath(shared_from_this()) &&
				_to && _to->addPath(shared_from_this());
		}

		bool Path::_release()
		{
			return _from && _from->removePath(shared_from_this()) &&
				_to && _to->removePath(shared_from_this());
		}

		P_Path Path::connect(P_Pin &from, P_Pin &to, const std::string &name /*= ""*/)
		{
			if (!from || !to)
			{
				return nullptr;
			}
			P_Spore fromSpore = from->spore();
			P_Spore toSproe = to->spore();
			if (!fromSpore || !toSproe)
			{
				return nullptr;
			}
			if (fromSpore->parent().lock() == toSproe->parent().lock())
			{
				//两个Pin的宿主Spore为同级，通过Path连接
				P_Spore holder = fromSpore->parent().lock();
				if (!holder)
				{
					return nullptr;
				}
				if (from->type() != Pin_Type::OUT_PIN || to->type() != Pin_Type::IN_PIN)
				{
					return nullptr;
				}
				return  holder->create_or_find_Path(from, to, name);
			}
			else if (fromSpore == toSproe->parent().lock())
			{
				if (from->type() != Pin_Type::IN_PIN || to->type() != Pin_Type::IN_PIN)
				{
					return nullptr;
				}
				return  fromSpore->create_or_find_Path(from, to, name);
			}
			else if (fromSpore->parent().lock() == toSproe)
			{
				if (from->type() != Pin_Type::OUT_PIN || to->type() != Pin_Type::OUT_PIN)
				{
					return nullptr;
				}
				return  toSproe->create_or_find_Path(from, to, name);
			}
			return nullptr;
		}

		bool Path::release(const P_Path &path)
		{
			if (!path->from() || !path->to())
			{
				return false;
			}
			P_Spore fromSpore = path->from()->spore();
			P_Spore toSproe = path->to()->spore();
			if (!fromSpore || !toSproe)
			{
				return false;
			}
			if (fromSpore->parent().lock() == toSproe->parent().lock())
			{
				//两个Pin的宿主Spore为同级，通过Path连接
				P_Spore holder = fromSpore->parent().lock();
				if (!holder)
				{
					return nullptr;
				}
				return holder->deletePath(path);
			}
			else if (fromSpore == toSproe->parent().lock())
			{
				return fromSpore->deletePath(path);
			}
			else if (fromSpore->parent().lock() == toSproe)
			{
				return toSproe->deletePath(path);
			}
			return nullptr;
		}

		P_Pin Path::from() const
		{
			return _from;
		}

		P_Pin Path::to() const
		{
			return _to;
		}


		bool Path::isvalid() const
		{
			return _from != nullptr && _to != nullptr;
		}
	}
}

