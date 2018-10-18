#include <algorithm>
#include "Path.h"
#include "Pin.h"
#include "Spore.h"
namespace fs
{

	Path::Path(const std::string &name /*= ""*/):
		_name(name)
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

	P_Path Path::connect(P_Pin from, P_Pin to, const std::string &name /*= ""*/)
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
			return  fromSpore->create_or_find_Path(from, to, name);
		}
		return nullptr;
	}

	fs::P_Pin fs::Path::from() const
	{
		return _from;
	}

	P_Pin Path::to() const
	{
		return _to;
	}

	bool fs::Path::isvalid() const
	{
		return _from != nullptr && _to != nullptr;
	}

}

