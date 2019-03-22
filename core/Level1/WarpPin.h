#pragma once
#include <functional>
#include "Pin.h"
#include "Data.h"
#include "DataAdapter.h"

namespace fs
{
	namespace L1
	{
		template<typename DataType>
		class WarpPin : public Pin
		{
		public:
		    using ProcessType = std::function< void(Context&, DataType&) >;
			WarpPin(const std::string& name, Pin_Type type, ProcessType innerProcess) :
				Pin(name, type)
				, _innerProcess(innerProcess)
			{
			}
			virtual DataAdapter* adapter() override
			{
				return DataType::adapterInst();
			}
		protected:
			virtual bool enableProcess() const { return nullptr != _innerProcess; }
			virtual void process(Context& ct, const std::shared_ptr<Data>& pdata)
			{
				if (_innerProcess && pdata)
				{
					DataAdapter* adp = pdata->adapter();
					if (!adp || !adapter())
					{
						return;
					}
					DataType* pvs = nullptr;
					if (adp->hashCode() != adapter()->hashCode())
					{
						auto temp = adapter()->toData(adp->toAnyValues(pdata));
						pvs = (DataType*)temp.get();
						if(pvs)
						{
							_innerProcess(ct, *pvs);
						}
					}
					else
					{
						pvs = (DataType*)pdata.get();
						if(pvs)
						{
							_innerProcess(ct, *pvs);
						}
					}
				}
			}
		protected:
			ProcessType _innerProcess;
		};
	}
}
