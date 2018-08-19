#pragma once
#include <memory>
#include <vector>
#include <tuple>
#include "Object.h"

namespace fs
{

	enum class Pin_Type : unsigned int
	{
		UNKNOW = 0,
		IN_PIN,
		OUT_PIN
	};

	class Spore;
	using P_Spore = std::shared_ptr<Spore>;

	class Path;
	using P_Path = std::shared_ptr<Path>;

	class Pin;
	using P_Pin = std::shared_ptr<Pin>;

	class Pin : public Object<Pin, Spore>
	{
	public:
		~Pin();
		void push(C_Data data);
		std::tuple<bool, P_Path> connect(std::shared_ptr<Pin> otherPin, const std::string &pathName = "");
		Pin_Type type() const;
		std::vector<P_Path> paths();
	protected:
		using Object::Object;
		Pin(const Pin&) = delete;
		Pin& operator=(const Pin&) = delete;
	protected:
		Pin_Type _type;
		std::vector<P_Path> _paths;
		int _indexInSpore;
		friend class Spore;
	};
}


