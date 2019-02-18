#pragma once
#include <cassert>
#include <memory>
#include <functional>
#include "DataAdapter.h"
#include "Pin.h"
namespace fs
{
	namespace L1
	{
		class Context;
		class Data;
		template<typename DataSetType>
		class LightSchema
		{
		public:
			using InnerProcessType = std::function< void(Context&, typename DataSetType&)>;
			static void callInnerProcess(InnerProcessType innerProcess, Context& context, const std::shared_ptr<Data>& pdata)
			{
				DataSetType* pvs = (DataSetType*)pdata.get();
				if (innerProcess && pvs)
				{
					innerProcess(context, *pvs);
				}
			}

			virtual std::shared_ptr<Data> toData(const AnyValues &values) override
			{
				return std::shared_ptr<Data>((Data*)(toDataFunc(values)));
			}

			virtual AnyValues toAnyValues(const std::shared_ptr<Data>& pdata) override
			{
				DataSetType* p = (DataSetType*)pdata.get();
				if (p)
					return toAnyValuesFunc(*p);
				return AnyValues();
			}
		protected:
			virtual DataSetType* toDataFunc(const AnyValues &)
			{
				return nullptr;
			}

			virtual AnyValues toAnyValuesFunc(const DataSetType &)
			{
				return AnyValues();
			}
		protected:
			friend Spore;
		};
	}
}

