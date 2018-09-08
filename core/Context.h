#pragma once
#include "Object.h"
namespace fs
{
	class Spore;
	using P_Spore = std::shared_ptr<Spore>;
	class CellContext;
	class Context
	{
	public:
		Context(P_Spore ps, P_Session pss, P_AnyValues plocal);
		~Context();

		AnyValues& local;
		AnyValues& session;
		P_Data newData();
		P_Data newData(P_Data &&pdata);
		bool push(const std::string  &outPinName, P_Data ppack);
	protected:
		P_Spore _ps;
		P_Session _pss;
		P_AnyValues _plocal;

		friend class CellContext;
	};
}


