//#include <algorithm>
//#include "Object.h"
//
//fs::DataPack::DataPack(P_Session pss, const AnyValues &any):
//	_session(pss)
//{
//	if (any.size() > 0)
//	{
//		(*static_cast<AnyValues*>(this)) = any;
//	}
//}
//
//fs::P_Session fs::DataPack::getSession() const
//{
//	return _session;
//}
//
//
//std::atomic<long long int> fs::Traceable::_total = { 0 };
//std::atomic<fs::IdType> fs::Traceable::__baseID = { 0 };
//
//fs::Traceable::Traceable()
//{
//	_total++;
//}
//
//fs::Traceable::~Traceable()
//{
//	_total--;
//}
//
//long long int fs::Traceable::total()
//{
//	return _total;
//}
//
//fs::IdType fs::Traceable::newID(IdType preID)
//{
//	if (preID == 0)
//	{
//		return __baseID++;
//	}
//	__baseID = std::max(preID, __baseID.load());
//	return preID;
//}