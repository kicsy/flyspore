#pragma once
#include <string>
#include <memory>
#include <shared_mutex>
#include "AnyValues.h"
#include "Statement.h"
namespace fs
{
	namespace L1
	{
		class PropertyNode :public std::enable_shared_from_this<PropertyNode>
		{
		public:
			PropertyNode(){}
			~PropertyNode(){}
			PW_PropertyNode parent()
			{
				return _parent;
			}
			std::string name()
			{
				return _name;
			}
			AnyValues& propertys()
			{
				return _propertys;
			}

			std::vector<P_PropertyNode> childs()
			{
				std::shared_lock<std::shared_mutex> lock(_childs_mutex);
				std::vector<P_PropertyNode> cl;
				for (auto& x : _childs)
				{
					cl.push_back(x.second);
				}
				return cl;
			}
			P_PropertyNode getChild(const std::string &name)
			{
				std::shared_lock<std::shared_mutex> lock(_childs_mutex);
				auto iter = _childs.find(name);
				if (iter != _childs.end())
				{
					return (*iter).second;
				}
				return nullptr;
			}
			P_PropertyNode addChild(P_PropertyNode child)
			{
				if (!child)
					return nullptr;
				auto iter = _childs.find(child->_name);
				if (iter != _childs.end())
				{
					return nullptr;
				}
				child->_parent = weak_from_this();
				std::unique_lock<std::shared_mutex> lock(_childs_mutex);
				_childs[child->_name] = (child);
				return child;
			}
			P_PropertyNode addChild(const std::string &name)
			{
				return addChild(std::make_shared<PropertyNode>(name));
			}
			P_PropertyNode removeChild(P_PropertyNode child)
			{
				if (!child)
				{
					return nullptr;
				}
				return removeChild(child->_name);

			}
			P_PropertyNode removeChild(const std::string &name)
			{
				std::unique_lock<std::shared_mutex> lock(_childs_mutex);
				auto iter = _childs.find(name);
				if (iter == _childs.end())
				{
					return nullptr;
				}
				_childs.erase(iter);
				(*iter).second->_parent = nullptr;
				return (*iter).second;
			}

			std::string getLocator()
			{
				std::string loc;
				P_PropertyNode parent = _parent.lock();
				while (parent)
				{
					loc = (parent->name + "/") + loc;
				}
				return loc + _name;
			}
		protected:
			PW_PropertyNode _parent;
			std::string _name;
			AnyValues _propertys;
			std::unordered_map<std::string, P_PropertyNode> _childs;
			std::shared_mutex _childs_mutex;
		};
	}
}