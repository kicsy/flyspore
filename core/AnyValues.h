#pragma once
#include <memory>
#include <unordered_map>
#include <any>
namespace fs
{
	class AnyValues 
	{
	public:
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

		template<typename vT>
		bool get(const std::string &strKey,  vT& refValue) const
		{
			using storageType = std::shared_ptr<vT>;
			const std::any& ref = _values.at(strKey);
			if (!ref.has_value() || ref.type() != typeid(storageType))
			{
				return false;
			}
			refValue = *(std::any_cast<const storageType&>(ref).get());
			return true;
		}

		template<typename vT>
		bool is_a(const std::string &strKey) const
		{
			if (_values.count(strKey) <= 0)
			{
				return false;
			}
			const std::any& ref = _values.at(strKey);
			return ref.type() == typeid(vT);
		}

		bool isEmpty() const
		{
			return _values.size() == 0;
		}
	protected:
		std::unordered_map<std::string, std::any> _values;
	};
}
