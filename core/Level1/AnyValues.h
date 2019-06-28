#pragma once
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <any>
namespace fs
{
	namespace L1
	{
		class AnyValuesLock;
		class AnyValues
		{
		public:
			AnyValues():_values(1)
			{

			}
			AnyValues(const AnyValues& other)
			{
				std::shared_lock<std::shared_mutex> lock(other._mutex);
				_values = other._values;
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

			template<typename vT>
			vT get(const std::string &strKey) const
			{
				std::shared_lock<std::shared_mutex> lock(_mutex);
				using storageType = const std::shared_ptr<vT>;
				try
				{
					const std::any& ref = _values.at(strKey);
					if (!ref.has_value() || ref.type() != typeid(storageType))
					{
						return vT();
					}
					return *std::any_cast<storageType&>(ref).get();
				}
				catch (std::out_of_range )
				{
					return vT();
				}
			}

			template<typename vT>
			void set(const std::string &strKey, const vT& value)
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				ref<vT>(strKey) = value;
			}

			template<typename vT>
			bool is_a(const std::string &strKey) const
			{
				std::shared_lock<std::shared_mutex> lock(_mutex);
				if (_values.count(strKey) <= 0)
				{
					return false;
				}
				const std::any& ref = _values.at(strKey);
				return ref.type() == typeid(vT);
			}

			bool erase(const std::string &strKey)
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				auto iter = _values.find(strKey);
				if (iter != _values.end())
				{
					_values.erase(strKey);
					return true;
				}
				return false;
			}

			bool isEmpty() const
			{
				std::shared_lock<std::shared_mutex> lock(_mutex);
				return _values.size() == 0;
			}
		protected:
			template<typename vT>
			vT& ref(const std::string &strKey)
			{
				using storageType = std::shared_ptr<vT>;
				std::any& ref = _values[strKey];
				if (!ref.has_value())
				{
					ref = std::make_shared<vT>();
				}
				if (ref.type() != typeid(storageType))
				{
					ref = (_values[strKey] = std::make_shared<vT>());
				}
				return *std::any_cast<storageType&>(ref).get();
			}
		protected:
			std::unordered_map<std::string, std::any> _values;
			mutable std::shared_mutex _mutex;
			friend class AnyValuesLock;
		};

		class AnyValuesLock
		{
		public:
			AnyValuesLock(AnyValues& anyValues):
				_anyValues(anyValues)
			{
				_anyValues._mutex.lock();
			}

			~AnyValuesLock()
			{
				_anyValues._mutex.unlock();
			}

			template<typename vT>
			vT& ref(const std::string &strKey)
			{
				return _anyValues.ref<vT>(strKey);
			}
		private:
			AnyValues& _anyValues;
		};

	}
}
