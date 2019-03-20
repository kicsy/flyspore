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
		template<typename DataType>
		class LightSchema
		{
		public:	
			typedef void(*InnerProcessFunc)(Context&, DataType&);
			using InnerProcessType = std::function< InnerProcessFunc >;
			static void callInnerProcess(InnerProcessType innerProcess, Context& context, const std::shared_ptr<Data>& pdata)
			{
				DataType* pvs = (DataType*)pdata.get();
				if (innerProcess && pvs)
				{
					innerProcess(context, *pvs);
				}
			}
		protected:
			friend Spore;
		};

		
	}
}

