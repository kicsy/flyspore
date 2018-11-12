#pragma once
#include "Statement.h"
#include "AnyValues.h"
namespace fs
{
	namespace L0
	{
		class DataAdapter
		{
		public:
			virtual ~DataAdapter() {}
			virtual bool equal(DataAdapter*) = 0;
			virtual P_Data toData(const AnyValues &values) = 0;
			virtual AnyValues toAnyValues(const P_Data& pdata) = 0;
		};
	}
}

