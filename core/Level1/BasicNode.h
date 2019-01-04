#pragma once
#include <string>
#include  <utility>
#include <memory>
#include <shared_mutex>
#include <any>
#include <unordered_map>
#include <vector>
#include "Statement.h"
namespace fs
{
	namespace L1
	{
		enum NodeMode : int16_t {
			Inherit =			0x0001 << 0,
			Necessity =		0x0001 << 1,
			Readable =		0x0001 << 2,
			Writable =		0x0001 << 3,
			Executable =		0x0001 << 4,
			Hide =				0x0001 << 5
		};


		using any = std::any;
		class BasicNode
		{
		public:

			BasicNode(any  &value = any())
			{
				_value = value;
			}
			BasicNode(any &&value)
			{
				std::swap(_value, value);
			}
			BasicNode(const BasicNode& other)
			{
				_value = other._value;
			}
			BasicNode(BasicNode&& other)
			{
				swap(std::forward<BasicNode>(other));
			}
			BasicNode& operator=(const BasicNode& other)
			{
				_value = other._value;
			}
			~BasicNode(){}
			PW_BasicNode parent() const 
			{
				std::shared_lock<const BasicNode> lock(*this);
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
				std::shared_lock<const BasicNode> lock(*this);
				return std::any_cast<vT>(_value);
			}

			template<typename vT>
			vT& ref()
			{
				return std::any_cast<vT&>(_value);
			}

			template<typename vT>
			const vT& ref() const 
			{
				return std::any_cast<vT&>(_value);
			}

			template<typename vT>
			void set(const vT& value)
			{
				std::unique_lock<BasicNode> lock(*this);
				_value = value;
			}

			template<typename vT>
			bool is_a() const
			{
				std::shared_lock<BasicNode> lock(*const_cast<BasicNode*>(this));
				return _value.type() == typeid(vT);
			}

			bool has_value()
			{
				return _value.has_value();
			}

			void swap(BasicNode &&other)
			{
				std::swap(_value, other._value);
				std::swap(_parent, other._parent);
				std::swap(_mode, other._mode);
			}
		protected:
			virtual PW_BasicNode  weakFromThis() = 0;
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
			PW_BasicNode _parent;
			any _value;
			std::shared_mutex _mutex;
			int16_t _mode{ 0 };
			friend class BasicNodeMap;
			friend class BasicNodeList;
			friend class std::shared_lock<BasicNode>;
		};

		template<class... _Types>
		P_BasicNode make_PropertyNode(_Types&&... _Args)
		{
			return std::make_shared<PropertyMode>(std::forward<_Types>(_Args)...);
		}

		class BasicNodeMap : public BasicNode
		{
		public:
			using innerValueType = std::unordered_map<std::string, P_BasicNode>;
			BasicNodeMap() :BasicNode(innerValueType())
			{
			}
			BasicNodeMap(std::unordered_map<std::string, std::any> values) :BasicNode(innerValueType())
			{
				for (auto& x : values)
				{
					(ref<innerValueType>())[x.first] = make_PropertyNode(x.second);
				}
			}
			BasicNodeMap(const innerValueType &&nodeMap) :
				BasicNode(nodeMap)
			{
				for (auto& x : ref<innerValueType>())
				{
					if (x.second)
					{
						(x.second)->_parent = weakFromThis();
					}
				}
			}
			std::vector<std::string> keys() const
			{
				std::shared_lock<const BasicNodeMap> lock(*this);
				std::vector<std::string> ks;
				for (auto& x : ref<innerValueType>())
				{
					ks.push_back(x.first);
				}
				return ks;
			}

			void foreach(std::function<bool(const P_BasicNode& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const BasicNodeMap> lock(*this);
					for (const auto& x : ref<const innerValueType>())
					{
						if (!p(x.second))
						{
							return;
						}
					}
				}
			}

			template<typename vT>
			vT get(const std::string &key) const
			{
				std::shared_lock<const BasicNodeMap> lock(*this);
				const innerValueType &map = ref<innerValueType>();
				const innerValueType::const_iterator iter = map.find(key);
				if(iter == map.end() || !iter->second)
				{
					return vT();
				}
				return iter->second->ref<vT>();
			}

			template<>
			P_BasicNode get<P_BasicNode>(const std::string &key) const
			{
				std::shared_lock<const BasicNodeMap> lock(*this);
				const innerValueType &map = ref<innerValueType>();
				const innerValueType::const_iterator iter = map.find(key);
				if (iter == map.end())
				{
					return nullptr;
				}
				return iter->second;
			}

			template<typename vT>
			void set(const std::string &key, vT&& node)
			{
				std::unique_lock<BasicNodeMap> lock(*this);
				((*this)[key] = make_PropertyNode(std::any(std::forward<vT>(node))))->_parent = weakFromThis();
			}

			template<>
			void set<P_BasicNode>(const std::string &key, P_BasicNode&& node)
			{
				std::unique_lock<BasicNodeMap> lock(*this);
				if (!node)
				{
					return;
				}
				((ref<innerValueType>())[key] = node)->_parent = weakFromThis();
			}

			template<typename vT>
			vT& operator[](const std::string &key)
			{
				innerValueType &map = ref<innerValueType>();
				innerValueType::iterator iter = map.find(key);
				if (iter != map.end() && iter->second)
				{
					return iter->second->ref<vT>();
				}
				return std::move(vT());
			}

			template<>
			P_BasicNode& operator[]<P_BasicNode>(const std::string &key)
			{
				innerValueType &map = ref<innerValueType>();
				innerValueType::iterator iter = map.find(key);
				if (iter != map.end())
				{
					return iter->second;
				}
				return std::move(P_BasicNode());
			}

			bool contains(const std::string &key) const
			{
				std::shared_lock<const BasicNodeMap> lock(*this);
				const innerValueType &map = ref<innerValueType>();
				return map.find(key) != map.end();
			}

			P_BasicNode remove(const std::string &key)
			{
				P_BasicNode node;
				std::unique_lock<BasicNodeMap> lock(*this);
				innerValueType &map = ref<innerValueType>();
				innerValueType::const_iterator iter = map.find(key);
				if (iter != map.end())
				{
					node = iter->second;
					map.erase(iter);
				}
				return std::move(node);
			}
		protected:
			template<typename vT> vT get() const = delete;
			template<typename vT> void set(const vT&) = delete;
			template<typename vT> bool is_a() const = delete;
			bool has_value() = delete;
		};

		template<class... _Types>
		P_BasicNode make_PropertyMap(_Types&&... _Args)
		{
			return  std::dynamic_pointer_cast<BasicNode>(std::make_shared<BasicNodeMap>(std::forward<_Types>(_Args)...));
		}


		class BasicNodeList : public BasicNode
		{
		public:
			using innerValueType = std::vector<P_BasicNode>;

			BasicNodeList() :BasicNode(innerValueType())
			{
			}

			BasicNodeList(std::list<std::any> values) :BasicNode(innerValueType())
			{
				for (auto& x : values)
				{
					(ref<innerValueType>()).push_back(make_PropertyNode(x));
				}
			}

			BasicNodeList(const innerValueType &&nodeMap) :
				BasicNode(nodeMap)
			{
				std::shared_lock<const BasicNodeList> lock(*this);
				for (auto& x : ref<innerValueType>())
				{
					if (x)
					{
						x->_parent = weakFromThis();
					}
				}
			}

			unsigned int length() const
			{
				return ref<innerValueType>().size();
			}

			void foreach(std::function<bool(const P_BasicNode& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const BasicNodeList> lock(*this);
					for (const auto& x : ref<const innerValueType>())
					{
						if (!p(x))
						{
							return;
						}
					}
				}
			}

			template<typename vT>
			vT get(innerValueType::size_type index) const
			{
				std::shared_lock<const BasicNodeList> lock(*this);
				const innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size() || !v.at(index))
				{
					return vT();
				}
				return v.at(index)->ref<vT>();
			}

			template<>
			P_BasicNode get<P_BasicNode>(innerValueType::size_type index) const
			{
				std::shared_lock<const BasicNodeList> lock(*this);
				const innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size())
				{
					return nullptr;
				}
				return v.at(index);
			}

			template<typename vT>
			void set(innerValueType::size_type index, vT&& node)
			{
				std::unique_lock<BasicNodeList> lock(*this);
				((*this)[index] = make_PropertyNode(std::any(std::forward<vT>(node))))->_parent = weakFromThis();
			}

			template<>
			void set<P_BasicNode>(innerValueType::size_type index, P_BasicNode&& node)
			{
				std::unique_lock<BasicNodeList> lock(*this);
				if (!node)
				{
					return;
				}
				innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size())
				{
					return;
				}
				((ref<innerValueType>())[index] = node)->_parent = weakFromThis();
			}

			template<typename vT>
			vT& operator[](innerValueType::size_type index)
			{
				innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size() || !v.at(index))
				{
					return std::move(vT());
				}
				return v.at(index)->ref<vT>();
			}

			template<>
			P_BasicNode& operator[]<P_BasicNode>(innerValueType::size_type index)
			{
				innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size() || !v.at(index))
				{
					return std::move(P_BasicNode());
				}
				return v.at(index);
			}

			P_BasicNode remove(innerValueType::size_type index)
			{
				P_BasicNode node;
				std::unique_lock<BasicNodeList> lock(*this);
				innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size())
				{
					return nullptr;
				}
				node = v[index];
				v.erase(v.begin() + index);
				return std::move(node);
			}
		protected:
			template<typename vT> vT get() const = delete;
			template<typename vT> void set(const vT&) = delete;
			template<typename vT> bool is_a() const = delete;
			bool has_value() = delete;
		};

		template<class... _Types>
		P_BasicNode make_PropertyList(_Types&&... _Args)
		{
			return  std::dynamic_pointer_cast<BasicNode>(std::make_shared<BasicNodeList>(std::forward<_Types>(_Args)...));
		}

	}

}