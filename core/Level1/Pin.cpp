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
				Actuator::get_instance().emplace(std::bind(Pin::call_process, shared_from_this(), data));
			}

			_outPaths.for_each([&](const std::shared_ptr<Path>& path) {
				if (path != nullptr && path->isvalid())
				{
					path->move(data);
				}
			});

			//for (auto &path : _outPaths)
			//{
			//	if (path != nullptr && path->isvalid())
			//	{
			//		path->move(data);
			//	}
			//}

			//!!这里用到Nest的唯一共享锁，会导致性能瓶颈
			//auto theSpore = _spore.lock();
			//if (theSpore)
			//{
			//	auto nest = theSpore->nest();
			//	if (nest)
			//	{
			//		NestSharedLock lock(nest);
			//		for (auto &path : _outPaths)
			//		{
			//			if (path != nullptr && path->isvalid())
			//			{
			//				path->move(data);
			//			}
			//		}
			//	}
			//}
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

		void Pin::call_process(const std::shared_ptr<Pin>& pin, const std::shared_ptr<Data>&data)
		{
			if (pin)
			{
				pin->task_process(data);
			}
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

