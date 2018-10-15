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
		vT& ref(const std::string &strKey) throw
		{
			auto iter = find(strKey);
			if (iter != end())
			{
				try
				{
					return std::any_cast<vT&>(*iter);
				}
				catch (const std::bad_any_cast &){}
			}
			auto ret = emplace(std::make_pair(std::string(strKey), vT()))
			if (!ret.second){
				throw std::invalid_argument();
			}
			return std::any_cast<vT&>(*(ret.first));
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
