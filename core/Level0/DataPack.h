#pragma once
#include "Statement.h"
#include "Data.h"
#include "AnyValues.h"

namespace fs
{
	namespace L0
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
		};
	}
}