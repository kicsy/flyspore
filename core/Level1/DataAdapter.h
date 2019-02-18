#pragma once
#include "AnyValues.h"
namespace fs
{
	namespace L1
	{
		class Data;
		class DataAdapter
		{
		public:
			virtual ~DataAdapter() {}
			virtual unsigned int hashCode() const  = 0;
			virtual std::shared_ptr<Data> toData(const AnyValues &values) const = 0;
			virtual AnyValues toAnyValues(const std::shared_ptr<Data>& pdata) const = 0;
		};
	}
}

