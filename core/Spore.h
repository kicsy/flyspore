#pragma once
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include "Pin.h"
#include "Object.h"

namespace fs
{
	class Pin;
	class DataPack;
	class Context;
	class Spore;
	using P_Spore = std::shared_ptr<Spore>;

	using Pin_Process = std::function< void(Context& cc, C_Data data)>;
	class Spore : public Object<Spore, Spore>
	{
	public:
		Spore(const std::string &name, IdType id = 0);
		Spore(const Spore&) = delete;
		Spore() = delete;
		~Spore() {}
		bool input(P_Pin pin, C_Data data);
		std::vector<P_Pin> pins();
		P_Pin getPin(const std::string &name, Pin_Type type = Pin_Type::UNKNOW);
		P_Pin addPIn(const std::string &pinName, Pin_Type type, Pin_Process process = nullptr);
		P_Pin addInPIn(const std::string &pinName, Pin_Process process = nullptr);
		P_Pin addOutPIn(const std::string &pinName);

		template<typename ST, typename... Args>
		std::shared_ptr<ST> addChild(Args&&... args);
		P_Spore addChild(const std::string  &name, IdType id = 0);
		P_Session newSession(const std::string &entryPinName, const std::string &name = "");
		void cleanSession(P_Session pss);
		static P_Spore newSpore(const std::string  &name = "", IdType id = 0);

	protected:
		void buildSession(IdType sessionId);
		void releaseSession(IdType sessionId);
		void process(Pin_Process pprocess, C_Data data);
	protected:

		std::vector<P_Pin> _pins;
		std::vector<Pin_Process> _handler;
		//value container for each session
		std::unordered_map<IdType, P_AnyValues> _sessionValues;
		//as a read/write lock for session operation
		std::shared_mutex _shared_mutex_session;
		friend class Session;
	};

	template<typename ST, typename... Args>
	inline std::shared_ptr<ST> Spore::addChild(Args&&... args)
	{
		std::shared_ptr<ST> spore = std::make_shared<ST>(std::forward<Args>(args)...);
		_childs.push_back(spore);
		return std::move(spore);
	}

}


