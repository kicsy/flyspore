#pragma once
#include "Schema.h"
#include "DataPack.h"
namespace fs
{
	namespace L1
	{
		class DefaultSchema : public LightSchema<DataPack>
		{
		public:
			virtual unsigned int hashCode() override
			{
				return *(const unsigned int*)("DfSchema");
			}
		protected:
			virtual DataPack* toDataFunc(const AnyValues & values) override
			{
				return new DataPack(values);
			}

			virtual AnyValues toAnyValuesFunc(const DataPack & dataPack) override
			{
				return *(AnyValues*)(&dataPack);
			}
		};
	}
}
