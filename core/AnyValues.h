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
			std::any& ref = _values[strKey];
			if (!ref.has_value())
			{
				ref = vT();
			}
			if (ref.type() != typeid(vT))
			{
				ref = (_values[strKey] = vT());
			}
			return std::any_cast<vT&>(ref);
		}

		template<typename vT>
		bool is_a(const std::string &strKey) const
		{
			if (count(strKey) <= 0)
			{
				return false;
			}
			return _values[strKey].type() == typeid(vT);
		}

		bool isEmpty() const
		{
			return _values.size() == 0;
		}
	protected:
		std::unordered_map<std::string, std::any> _values;
	};
}
