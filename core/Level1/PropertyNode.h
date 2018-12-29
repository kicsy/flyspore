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
		enum PropertyMode : int16_t {
			Inherit =			0x0001 << 0,
			Necessity =		0x0001 << 1,
			Readable =		0x0001 << 2,
			Writable =		0x0001 << 3,
			Executable =		0x0001 << 4,
			Hide =				0x0001 << 5
		};


		using any = std::any;
		class PropertyNode
		{
		public:
			friend class std::shared_lock<PropertyNode>;
			PropertyNode(any &&value = any())
			{
				std::swap(_value, value);
			}
			PropertyNode(const PropertyNode& other)
			{
				_value = other._value;
			}
			PropertyNode(PropertyNode&& other)
			{
				swap(std::forward<PropertyNode>(other));
			}
			PropertyNode& operator=(const PropertyNode& other)
			{
				_value = other._value;
			}
			~PropertyNode(){}
			PW_PropertyNode parent() const 
			{
				std::shared_lock<const PropertyNode> lock(*this);
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
				std::shared_lock<const PropertyNode> lock(*this);
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
				std::unique_lock<PropertyNode> lock(*this);
				_value = value;
			}

			template<typename vT>
			bool is_a() const
			{
				std::shared_lock<PropertyNode> lock(*const_cast<PropertyNode*>(this));
				return _value.type() == typeid(vT);
			}

			void swap(PropertyNode &&other)
			{
				std::swap(_value, other._value);
				std::swap(_parent, other._parent);
				std::swap(_mode, other._mode);
			}
		protected:
			virtual PW_PropertyNode  weakFromThis()
			{
				return PW_PropertyNode();
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
			PW_PropertyNode _parent;
			any _value;
			std::shared_mutex _mutex;
			int16_t _mode{ 0 };
			friend class PropertyMap;
			friend class PropertyList;
		};

		class PropertyMap : public PropertyNode
		{
		public:
			using innerValueType = std::unordered_map<std::string, PropertyNode>;
			PropertyMap(const innerValueType &&nodeMap) :
				PropertyNode(nodeMap)
			{
				std::shared_lock<const PropertyMap> lock(*this);
				for (auto& x : ref<innerValueType>())
				{
					x.second._parent = weakFromThis();
				}
			}
			std::vector<std::string> keys() const
			{
				std::shared_lock<const PropertyMap> lock(*this);
				std::vector<std::string> ks;
				for (auto& x : ref<innerValueType>())
				{
					ks.push_back(x.first);
				}
				return ks;
			}

			void foreach(std::function<bool(const PropertyNode& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const PropertyMap> lock(*this);
					for (const auto& x : ref<const innerValueType>())
					{
						if (!p(x.second))
						{
							return;
						}
					}
				}
			}

			PropertyNode get(const std::string &key) const
			{
				std::shared_lock<const PropertyMap> lock(*this);
				const innerValueType &map = ref<innerValueType>();
				const innerValueType::const_iterator iter = map.find(key);
				if(iter == map.end())
				{
					return std::move(PropertyNode());
				}
				return std::move(iter->second);
			}

			void set(const std::string &key, const PropertyNode& node)
			{
				std::unique_lock<PropertyMap> lock(*this);
				((*this)[key] = node)._parent = weakFromThis();
			}

			PropertyNode& operator[](const std::string &key)
			{
				return ref<innerValueType>()[key];
			}

			bool contains(const std::string &key) const
			{
				std::shared_lock<const PropertyMap> lock(*this);
				const innerValueType &map = ref<innerValueType>();
				return map.find(key) != map.end();
			}

			PropertyNode remove(const std::string &key)
			{
				PropertyNode node;
				std::unique_lock<PropertyMap> lock(*this);
				innerValueType &map = ref<innerValueType>();
				innerValueType::const_iterator iter = map.find(key);
				if (iter != map.end())
				{
					node = iter->second;
					map.erase(iter);
				}
				return node;
			}
		protected:
		};

		class PropertyList : public PropertyNode
		{
		public:
			using innerValueType = std::vector<PropertyNode>;
			PropertyList(const innerValueType &&nodeMap) :
				PropertyNode(nodeMap)
			{
				std::shared_lock<const PropertyList> lock(*this);
				for (auto& x : ref<innerValueType>())
				{
					x._parent = weakFromThis();
				}
			}

			unsigned int length() const
			{
				return ref<innerValueType>.size();
			}

			void foreach(std::function<bool(const PropertyNode& child)> p) const
			{
				if (p)
				{
					std::shared_lock<const PropertyList> lock(*this);
					for (const auto& x : ref<const innerValueType>())
					{
						if (!p(x))
						{
							return;
						}
					}
				}
			}

			PropertyNode get(innerValueType::size_type index) const
			{
				std::shared_lock<const PropertyList> lock(*this);
				const innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size())
				{
					return PropertyNode();
				}
				return v.at(index);
			}

			void set(innerValueType::size_type index, const PropertyNode& node)
			{
				std::unique_lock<PropertyList> lock(*this);
				((*this)[index] = node)._parent = weakFromThis();
			}

			PropertyNode& operator[](innerValueType::size_type index)
			{
				return ref<innerValueType>()[index];
			}

			PropertyNode remove(innerValueType::size_type index)
			{
				PropertyNode node;
				std::unique_lock<PropertyList> lock(*this);
				innerValueType &v = ref<innerValueType>();
				if (index < 0 || index >= v.size())
				{
					return PropertyNode();
				}
				node = v[index];
				v.erase(v.begin() + index);
				return node;
			}
		protected:
		};
	}

}