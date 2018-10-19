#pragma once
#include <memory>
#include <unordered_map>
#include <any>
namespace fs
{
	class AnyValues : public std::unordered_map<std::string, std::any>
	{
	public:
		AnyValues() = default;
		virtual ~AnyValues();
		AnyValues(const AnyValues&) = default;

		template<typename vT>
		vT& ref(const std::string &strKey)
		{
			try
			{
				return std::any_cast<vT&>(at(strKey));
			}
			catch (const std::out_of_range&)
			{
				std::any& ref = ((*this)[strKey] = vT());
				return std::any_cast<vT&>(ref);
			}
		}

		template<typename vT>
		bool is_a(const std::string &strKey) const
		{
			if (count(strKey) <= 0)
			{
				return false;
			}
			return (*this)[strKey].type() == typeid(vT);
		}
	};
}
