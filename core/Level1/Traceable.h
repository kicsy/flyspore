#pragma once
#include <memory>
#include <shared_mutex>

namespace fs
{
	namespace L1
	{
		template<typename BasicType>
		class Traceable : public std::enable_shared_from_this<BasicType>
		{
			using P_BasicType = std::shared_ptr<BasicType>;

		public:
			Traceable(const std::string & name):_name(name){}
			~Traceable(){}

			std::vector<P_BasicType> childs()
			{
				std::shared_lock<std::shared_mutex> lock(_childs_mutex);
				std::vector<P_BasicType> cl;
				for (auto& x : _childs)
				{
					cl.push_back(x.second);
				}
				return cl;
			}
			P_BasicType getChild(const std::string &name)
			{
				std::shared_lock<std::shared_mutex> lock(_childs_mutex);
				auto iter = _childs.find(name);
				if (iter != _childs.end())
				{
					return *iter;
				}
				return nullptr;
			}
			P_BasicType addChild(P_BasicType child)
			{
				if (!child)
					return nullptr;
				child->_parent = shared_from_this();
				std::unique_lock<std::shared_mutex> lock(_childs_mutex);
				_childs.push_back(child);
				return child;
			}
			P_BasicType addChild(const std::string &name)
			{
				return addChild(std::make_shared<BasicType>(name));
			}
			P_BasicType removeChild(BasicType child)
			{
				if (!child)
				{
					return nullptr;
				}
				return removeChild(child->name());

			}
			P_BasicType removeChild(const std::string &name)
			{
				std::unique_lock<std::shared_mutex> lock(_childs_mutex);
				auto iter = _childs.find(name);
				if (iter == _childs.end())
				{
					return nullptr;
				}
				_childs.erase(iter);
				(*iter)->_parent = nullptr;
				return *iter;
			}

		protected:
			std::string _name;
			BasicType _parent;
			std::unordered_map<std::string, P_BasicType> _childs;
			std::shared_mutex _childs_mutex;
		};
	}
}
