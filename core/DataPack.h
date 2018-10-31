#include "Statement.h"
#include "Data.h"
#include "AnyValues.h"

namespace fs
{
	class DataPack : public Data,  public AnyValues
	{
	public:
		DataPack(const P_Session &session, const AnyValues &any = AnyValues()) :
			Data(session)
			, AnyValues(any){}
		DataPack(const AnyValues &any = AnyValues()) :
			Data(NULL)
			,AnyValues(any) {}
		virtual ~DataPack() {}
		DataPack(const DataPack&) = delete;
		DataPack& operator=(const DataPack&) = delete;
	};
}