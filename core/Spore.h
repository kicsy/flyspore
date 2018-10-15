/*!
 * \file Spore.h
 * \date 2016/08/19 12:38
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include "Statement.h"
#include "Pin.h"

namespace fs
{
	class Spore : public std::enable_shared_from_this<Spore>
	{
	public:
		Spore();
		Spore(const Spore&) = delete;
		Spore() = delete;
		~Spore();
		PW_Spore parent();

		bool input(P_Pin pin, P_Data data);
		std::vector<P_Pin> pins();
		P_Pin addPIn(Pin_Type type, Pin_Process process = nullptr);

		std::vector<P_Spore> childs();
		P_Spore addChild(P_Spore child);

		template<typename ST, typename... Args>
		std::shared_ptr<ST> addChild(Args&&... args);
		P_Spore addChild(const std::string  &name, IdType id = 0);
		static P_Spore newSpore(const std::string  &name = "", IdType id = 0);

	protected:
		void buildSession(IdType sessionId);
		void releaseSession(IdType sessionId);
		void process(Pin_Process pprocess, P_Data data);
		P_Path create_or_find_Path(P_Pin from, P_Pin to);

	protected:
		P_Spore _parent;

		std::vector<P_Spore> _childs;
		std::shared_mutex _childs_mutex;


		std::vector<P_Pin> _pins;
		std::shared_mutex _pins_mutex;

		std::vector<P_Path> _paths;
		std::shared_mutex _paths_mutex;


		//value container for each session
		std::unordered_map<IdType, P_AnyValues> _sessionValues;
		//as a read/write lock for session operation
		std::shared_mutex _session_mutex;
		friend class Session;
		friend class Path;
	};

	template<typename ST, typename... Args>
	inline std::shared_ptr<ST> Spore::addChild(Args&&... args)
	{
		std::shared_ptr<ST> spore = std::make_shared<ST>(std::forward<Args>(args)...);
		_childs.push_back(spore);
		return std::move(spore);
	}

}


