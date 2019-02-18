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
		class Node
		{
		public:
			friend class std::shared_lock<Node>;
			Node(const any &&value = any())
			{
			}

			~Node(){}
			PW_Node parent() const
			{
				std::shared_lock<Node> lock(*const_cast<Node*>(this));
				return _parent;
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
				std::shared_lock<Node> lock(*const_cast<Node*>(this));
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
			void set(const vT&& value)
			{
				std::unique_lock<Node> lock(*this);
				_value = any(value);
			}

			template<typename vT>
			bool is_a() const
			{
				std::shared_lock<Node> lock(*const_cast<Node*>(this));
				return _value.type() == typeid(vT);
			}
		protected:
			virtual PW_Node  weakFromThis() 
			{
				return PW_Node();
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
			PW_Node _parent;
			any _value;
			std::shared_mutex _mutex;
			int16_t _mode{ 0 };
		};

		class NodeMap : public Node
		{
		public:
			using container = std::unordered_map<std::string, Node>;
			NodeMap(container nodemap) :
				Node(any(nodemap))
			{
			}

			std::vector<std::string> keys() const
			{
				std::shared_lock<const NodeMap> lock(*this);
				std::vector<std::string> cl;
				for (auto& x : ref<container>())
				{
					cl.push_back(x.first);
				}
				return std::move(cl);
			}

			void foreach(std::function<bool(const std::string &key, const Node& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const NodeMap> lock(*this);
					for (auto& x : ref<container>())
					{
						if (!p(x.first, x.second))
						{
							return;
						}
					}
				}
			}

			P_Property getItem(const std::string &name)
			{
				std::shared_lock<NodeMap> lock(*this);
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
				std::unique_lock<Node> lock(*const_cast<Node*>(_owner.lock().get()));
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
				std::unique_lock<Node> lock(*const_cast<Node*>(_owner.lock().get()));
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