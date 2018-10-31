#pragma once
#include <vector>
#include "Statement.h"
#include <shared_mutex>
namespace fs
{
	enum class Pin_Type : unsigned int
	{
		UNKNOW = 0,
		IN_PIN,
		OUT_PIN
	};

	class Pin : public std::enable_shared_from_this<Pin>
	{
	public:
		virtual ~Pin();
		void push(P_Data data);
		Pin_Type type() const;
		std::vector<P_Path> paths();
		P_Spore spore();
		std::string name();
	protected:
		Pin(PW_Spore spore, std::string name, Pin_Type type);
		Pin(const Pin&) = delete;
		Pin& operator=(const Pin&) = delete;
		bool addPath(P_Path path);
		virtual void process(Context& ct, P_Data& pdata){}
		virtual bool enableProcess() { return false; }
	protected:
		Pin_Type _type;
		std::string _name;
		std::shared_mutex _paths_mutex;
		std::vector<P_Path> _paths;
		PW_Spore _spore;

		friend class Spore;
		friend class Path;
	};
}


