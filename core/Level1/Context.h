#pragma once
#include "Statement.h"
namespace fs
{
	namespace L1
	{
		class CellContext;
		class Context
		{
		public:
			Context(P_Session pss);
			~Context();
			AnyValues& session;
			bool push(const std::string  &outPinName, const P_Data &ppack);
			P_Spore spore();
		protected:
			P_Session _pss;
			friend class CellContext;
		};
	}
}


