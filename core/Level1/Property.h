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
		class PropertyList;
		class Property
		{
		public:
			friend class std::shared_lock<Property>;
			friend class PropertyList;
			Property(const std::string & name, const any &&value = any()):
				_name(name)
			{
				_value = value;
				if (_value.type() == typeid(PropertyList))
				{
					std::any_cast<PropertyList&>(_value)._owner = weakFromThis();
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
			const vT& ref() const 
			{
				return std::any_cast<const vT&>(_value);
			}

			template<typename vT>
			void set(const vT& value)
			{
				std::unique_lock<Property> lock(*this);
				_value = any(value);
				if (value.type() == typeid(PropertyList))
				{
					std::any_cast<PropertyList&>(_value)._owner = weakFromThis();
				}
			}

			template<typename vT>
			bool is_a() const
			{
				std::shared_lock<Property> lock(*const_cast<Property*>(this));
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
			virtual PW_Property  weakFromThis() 
			{
				return PW_Property();
			}

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

		class PropertyCollection : public Property
		{
		public:
			using ItemList = std::list<P_Property>;
			PropertyCollection(const std::string &name, const ItemList&& itemList = ItemList()) :
				Property(name, any(std::forward<const ItemList>(itemList)))
			{
			}

			std::vector<std::string> itemNames() const
			{
				std::shared_lock<const PropertyCollection> lock(*this);
				std::vector<std::string> cl;
				for (auto& x : ref<ItemList>())
				{
					if (x)
					{
						cl.push_back(x->_name);
					}
				}
				return std::move(cl);
			}

			void foreach(std::function<bool(const Property& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const PropertyCollection> lock(*this);
					for (auto& x : ref<ItemList>())
					{
						if (x && !p(*x.get()))
						{
							return;
						}
					}
				}
			}

			P_Property getItem(const std::string &name)
			{
				std::shared_lock<PropertyCollection> lock(*this);
				auto iter = _items.find(name);
				if (iter != _items.end())
				{
					return (*iter).second;
				}
				return nullptr;
			}

			P_Property addItem(P_Property child)
			{
				if (!child)
				{
					return nullptr;
				}
				std::unique_lock<Property> lock(*const_cast<Property*>(_owner.lock().get()));
				auto iter = _items.find(child->_name);
				if (iter != _items.end())
				{
					return nullptr;
				}
				child->_parent = _owner;
				_items[child->_name] = (child);
				return child;
			}
			P_Property removeItem(P_Property child)
			{
				if (!child)
				{
					return nullptr;
				}
				return removeItem(child->_name);
			}
			P_Property removeItem(const std::string &name)
			{
				std::unique_lock<Property> lock(*const_cast<Property*>(_owner.lock().get()));
				auto iter = _items.find(name);
				if (iter == _items.end())
				{
					return nullptr;
				}
				_items.erase(iter);
				(*iter).second->_parent.reset();
				return (*iter).second;
			}
		protected:
		};
	}
}