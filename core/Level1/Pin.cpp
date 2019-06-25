#include <algorithm>
#include "Pin.h"
#include "Spore.h"
#include "DefaultNest.h"
#include "Session.h"
#include "Context.h"
#include "Path.h"
#include "Actuator.h"
namespace fs
{
	namespace L1
	{
		Pin::Pin(const std::string& name, Pin_Type type):
			_type(type)
			,_name(name)
		{
		}

		Pin::~Pin()
		{
		}

		bool Pin::push(const std::shared_ptr<Data> &data)
		{
			if (!data)
			{
				return false;
			}
			std::shared_ptr<Session> pss = data->getSession();
			if (!pss || pss->status() == Session_Status::BlackOut)
			{
				return false;
			}
			if (_type == Pin_Type::IN_PIN && enableProcess())
			{
				pss->increaseTask();
				Actuator::get_instance().emplace(std::bind(&Pin::task_process, this, data));
			}

			_outPaths.for_each([&](const std::shared_ptr<Path>& path) {
				if (path != nullptr && path->isvalid())
				{
					path->move(data);
				}
			});
			return true;
		}

		std::shared_ptr<fs::L1::Path> Pin::connect(const std::shared_ptr<Pin>& to, const std::string& name /*= ""*/)
		{
			return Path::connect(shared_from_this(), to, name);
		}

		void Pin::task_process(const std::shared_ptr<Data>&data)
		{
			std::shared_ptr<Session> pss = data->getSession();
			Context cc(spore(), pss);
			process(cc, data);
			pss->decreaseTask();
		}

		DataAdapter* Pin::adapter()
		{
			return nullptr;
		}

		Pin_Type Pin::type() const
		{
			return _type;
		}

		std::shared_ptr<Spore> Pin::spore() const
		{
			return _spore.lock();
		}

		std::string Pin::name() const
		{
			return _name;
		}
	}
}

