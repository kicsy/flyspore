#pragma once
#include <string>
#include  <utility>
#include <memory>
#include <shared_mutex>
#include <any>
#include <stack>
#include "Statement.h"
namespace fs
{
	namespace L1
	{
		enum PropertyMode : int16_t {
			Inherit =			0x0001 << 0,
			Necessity =		0x0001 << 1,
			Readable =		0x0001 << 2,
			Writable =		0x0001 << 3,
			Executable =		0x0001 << 4,
			Hide =				0x0001 << 5
		};


		using any = std::any;
		using pairItem = std::pair<std::string, any>;
		using PropertyInitList = std::list<pairItem>;

		class Property
		{
		public:
			friend class std::shared_lock<Property>;
			Property(const std::string & name, const any &&value = any(),
				const PropertyInitList &&initList = PropertyInitList()):
				_name(name)
				,_value(value)
			{
				for (auto &pairItem : initList)
				{
					if (pairItem.first.length() > 0)
					{
						_childs[pairItem.first] = 
							std::make_shared<Property>(pairItem.first, pairItem.second);
					}
				}
			}
			~Property(){}
			PW_Property parent() const 
			{
				std::shared_lock<Property> lock(*const_cast<Property*>(this));
				return _parent;
			}
			std::string name() const 
			{
				std::shared_lock<Property> lock(*const_cast<Property*>(this));
				return _name;
			}
			bool setName(const std::string &name)
			{
				if (name.length() == 0)
				{
					return false;
				}
				std::unique_lock<Property> lock(*this);
				_name = name;
				return true;
			}
			uint16_t mode() const 
			{
				return _mode;
			}
			uint16_t setMode(uint16_t mode)
			{
				_mode = mode;
			}
			template<typename vT> 
			vT get() const
			{
				std::shared_lock<Property> lock(*const_cast<Property*>(this));
				return std::any_cast<vT&>(_value);
			}

			template<typename vT>
			vT& ref()
			{
				return std::any_cast<vT&>(_value);
			}

			template<typename vT>
			void set(const vT& value)
			{
				std::unique_lock<Property> lock(*this);
				std::any_cast<vT&>(_value)  = value;
			}

			template<typename vT>
			bool is_a() const
			{
				std::shared_lock<Property> lock(*this);
				return _value.type() == typeid(vT);
			}

			Property& operator[] (const std::string &name)
			{
				P_Property pp = _childs[name];
				if (!pp)
				{
					_childs[name] = pp = std::make_shared<Property>(name);
				}
				return *pp.get();
			}
			std::vector<std::string> childNames() const
			{
				std::shared_lock<Property> lock(*const_cast<Property*>(this));
				std::vector<std::string> cl;
				for (auto& x : _childs)
				{
					if (x.second)
					{
						cl.push_back(x.second->_name);
					}
				}
				return std::move(cl);
			}

			void foreachChild(std::function<bool(const Property& child)> p) const
			{
				if (p)
				{
					std::shared_lock<Property> lock(*const_cast<Property*>(this));
					for (auto& x : _childs)
					{
						if (x.second && !p(*x.second.get()))
						{
							return;
						}
					}
				}
			}

			P_Property getChild(const std::string &name)
			{
				std::shared_lock<Property> lock(*this);
				auto iter = _childs.find(name);
				if (iter != _childs.end())
				{
					return (*iter).second;
				}
				return nullptr;
			}
			P_Property addChild(P_Property child)
			{
				if (!child)
				{
					return nullptr;
				}
				std::unique_lock<Property> lock(*this);
				auto iter = _childs.find(child->_name);
				if (iter != _childs.end())
				{
					return nullptr;
				}
				child->_parent = weakFromThis();
				_childs[child->_name] = (child);
				return child;
			}
			P_Property removeChild(P_Property child)
			{
				if (!child)
				{
					return nullptr;
				}
				return removeChild(child->_name);
			}
			P_Property removeChild(const std::string &name)
			{
				std::unique_lock<Property> lock(*this);
				auto iter = _childs.find(name);
				if (iter == _childs.end())
				{
					return nullptr;
				}
				_childs.erase(iter);
				(*iter).second->_parent.reset();
				return (*iter).second;
			}

			std::string getLocator()
			{
				std::string loc;
				P_Property parent = _parent.lock();
				while (parent)
				{
					loc = (parent->name + "/") + loc;
				}
				return loc + _name;
			}
		protected:
			virtual PW_Property  weakFromThis() = 0;

			virtual void visit()
			{
			}

			void touch()
			{
				visit();
				for (auto child : _childs)
				{
					child.second->touch();
				}
			}

			void lock()
			{
				_mutex.lock();
			}
			void unlock()
			{
				_mutex.unlock();
			}

			void lock_shared()
			{
				_mutex.lock_shared();
			}

			void unlock_shared()
			{
				_mutex.unlock_shared();
			}

			bool try_lock()
			{
				return _mutex.try_lock();
			}

			bool try_lock_shared()
			{
				return _mutex.try_lock_shared();
			}
		protected:
			PW_Property _parent;
			std::string _name;
			any _value;
			std::unordered_map<std::string, P_Property> _childs;
			std::shared_mutex _mutex;
			int16_t _mode{ 0 };
		};
	}
}