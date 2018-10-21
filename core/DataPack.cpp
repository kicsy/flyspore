#include "DataPack.h"

namespace fs
{
	DataPack::DataPack(P_Session pss, const AnyValues &any) :AnyValues()
		,_session(pss)
	{
		(*static_cast<AnyValues*>(this)) = any;
	}

	fs::P_Session fs::DataPack::getSession() const
	{
		return _session;
	}
}