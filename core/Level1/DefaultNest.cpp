#include "DefaultNest.h"
#include "Spore.h"
#include "Pin.h"
#include "Path.h"
namespace fs
{
	namespace L1
	{

		DefaultNest::DefaultNest()
		{
		}


		DefaultNest::~DefaultNest()
		{
		}

		std::shared_ptr<Spore> DefaultNest::createSpore()
		{
			auto pnode = std::shared_ptr<Spore>(new Spore(std::shared_ptr<DefaultNest>(shared_from_this(), this)));
			onCreated(std::static_pointer_cast<BasicNode>(pnode));
			return pnode;
		}

		std::shared_ptr<Pin> DefaultNest::createPin()
		{
			auto pnode = std::shared_ptr<Pin>(new Pin(std::shared_ptr<DefaultNest>(shared_from_this(), this)));
			onCreated(std::static_pointer_cast<BasicNode>(pnode));
			return pnode;

		}

		std::shared_ptr<Path> DefaultNest::connectPin(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string& name)
		{
			if (!from || !to)
			{
				return nullptr;
			}
			auto fromSpore = getParent(from);
			auto toSproe = getParent(to);
			if (!fromSpore || !toSproe)
			{
				return nullptr;
			}
			auto fromSporeParent = getParent(fromSpore);
			auto toSporeParent = getParent(toSproe);
			if (fromSporeParent == toSporeParent)
			{
				//两个Pin的宿主Spore为同级，通过Path连接
				if (!toSporeParent)
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

		std::shared_ptr<Spore> DefaultNest::getParent(std::shared_ptr<Pin> pin)
		{
			if (!pin)
			{
				return nullptr;
			}
			auto _p = pin->parent();
			if (!_p || !(_p->mode() & NodeMode::AsMap) || _p->mark<std::string>() != "pins")
			{
				return nullptr;
			}
			_p = _p->parent();
			auto _spore = std::dynamic_pointer_cast<Spore>(_p);
			return _spore;
		}

		std::shared_ptr<fs::L1::Spore> DefaultNest::getParent(std::shared_ptr<Spore> spore)
		{
			if (!spore)
			{
				return nullptr;
			}
			auto _p = spore->parent();
			if (!_p || !(_p->mode() & NodeMode::AsMap) || _p->mark<std::string>() != "/")
			{
				return nullptr;
			}
			_p = _p->parent();
			auto _spore = std::dynamic_pointer_cast<Spore>(_p);
			return _spore;
		}

	}
}