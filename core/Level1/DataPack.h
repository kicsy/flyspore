#pragma once
#include "Data.h"
#include "AnyValues.h"

namespace fs
{
	namespace L1
	{
		class DataPack : public Data, public AnyValues
		{
		public:
			DataPack(const AnyValues &any = AnyValues()) :
				Data(NULL)
				, AnyValues(any) {
			}
			virtual ~DataPack() {
			}
			DataPack(const DataPack&) = delete;
			DataPack& operator=(const DataPack&) = delete;
			virtual unsigned int hashCode() override
			{
				return 0;
			}
			static DataPack* toData(const AnyValues & values)
			{
				return new DataPack(values);
			}
			static AnyValues toAnyValues(const DataPack & dataPack)
			{
				return *(AnyValues*)(&dataPack);
			}
		};
	}
}