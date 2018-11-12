#pragma once
#include "Statement.h"
namespace fs
{
	namespace L0
	{
		class CellContext;
		class Context
		{
		public:
			Context(P_Spore ps, P_Session pss, P_AnyValues plocal);
			~Context();
			AnyValues& local;
			AnyValues& session;
			bool push(const std::string  &outPinName, const P_Data &ppack);
			P_Spore spore();
		protected:
			P_Spore _ps;
			P_Session _pss;
			P_AnyValues _plocal;

			friend class CellContext;
		};
	}
}


