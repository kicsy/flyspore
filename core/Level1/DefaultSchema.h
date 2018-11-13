#pragma once
#include "Schema.h"
#include "DataPack.h"
namespace fs
{
	namespace L1
	{
		class DefaultSchema : public LightSchema<DataPack>
		{
		protected:
			virtual DataPack* toDataFunc(const AnyValues & values)
			{
				return new DataPack(values);
			}

			virtual AnyValues toAnyValuesFunc(const DataPack & dataPack)
			{
				return *(AnyValues*)(&dataPack);
			}
		};

		DefaultSchema& defaultSchema();
	}
}
