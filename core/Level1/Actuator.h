#pragma once
#include "../Level0/workstealing_threadpool.hpp"
#include "../Level0/task_pool.h"
namespace fs
{
	namespace L1
	{
		class Actuator 
		{
		public:
			enum ThreadPoolType
			{
				Default = 0,
				WorkStealingThreadpool = 1
			};
			Actuator()
			{
			}
			void setType(ThreadPoolType type)
			{
				_type = type;
			}

			template<typename Func, typename... Args>
			auto submit(Func&& f, Args&&... args)
			{
				return _defaultThreadpool.submit(std::forward<Func>(f), std::forward<Args>(args)...);
			}

			void emplace(const std::function<void()> &func)
			{
				switch (_type)
				{
				case fs::L1::Actuator::Default:
					{
						_defaultThreadpool.submit(func);
					}
					break;
				case fs::L1::Actuator::WorkStealingThreadpool:
					{
						_workStealingThreadpool.emplace(func);
					}
					break;
				default:
					break;
				}
			}

			static Actuator& get_instance() {
				static Actuator _inst;
				return _inst;
			}

		protected:
			ThreadPoolType _type{ ThreadPoolType::Default };
			fs::L0::task_pool _defaultThreadpool;
			tf::WorkStealingThreadpool<std::function<void()>> _workStealingThreadpool{ std::thread::hardware_concurrency() };
		};
	}
}