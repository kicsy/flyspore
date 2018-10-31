#pragma once
#include <cassert>
#include "Statement.h"
#include "DataAdapter.h"
namespace fs
{
	template< typename ProcessType = void(Context&, DataPack&)>
	class Schema : public DataAdapter
	{
	public:
		using InnerProcessType = std::function<typename ProcessType>;
		using TransformFunc = void(InnerProcessType innerProcess, Context& context, P_Data& pdata);
		Schema() {}
		virtual ~Schema() {}

		virtual void callInnerProcess(InnerProcessType innerProcess, Context& context, P_Data& pdata)
		{
			//transform...
			DataPack* pvs = (DataPack*)pdata.get();
			if (innerProcess && pvs)
			{
				innerProcess(context, *pvs);
			}
		}

		virtual long long id()
		{
			return (long long)0;// (Schema<ProcessType>::callInnerProcess);
		}

		virtual bool equal(DataAdapter* other)
		{
			return other && id() == other->id();
		}
		virtual P_Data toData(const AnyValues &values)
		{
			return std::move(std::make_shared<DataPack>(values));
		}
		virtual AnyValues toAnyValues(const P_Data& pdata)
		{
			DataPack* p = (DataPack*)pdata.get();
			if (p)
			{
				return *(AnyValues*)p;
			}
			return std::move(AnyValues());
		}
	protected:
		friend Spore;
	};

}

