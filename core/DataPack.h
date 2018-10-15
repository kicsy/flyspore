#include "Statement.h"
#include "AnyValues.h"

namespace fs
{
	class DataPack : public AnyValues
	{
	public:
		~DataPack() {}
		DataPack(const DataPack&) = delete;
		DataPack& operator=(const DataPack&) = delete;
		P_Session getSession() const;
	protected:
		DataPack(P_Session pss, const AnyValues &any = AnyValues());
	protected:
		P_Session _session;
		friend class Session;
	};
}