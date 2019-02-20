#pragma once
#include "Pin.h"
#include "Data.h"
#include "DataPack.h"
#include "DataAdapter.h"
#include "Schema.h"

namespace fs
{
	namespace L1
	{
		template<typename DataType = DataPack, typename SchemaType = LightSchema<typename DataType>>
		class WarpPin : public Pin
		{
		public:
			using InnerProcessType = typename SchemaType::InnerProcessType;
			WarpPin(const std::string& name, Pin_Type type, InnerProcessType innerProcess) :
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

					if (adp->hashCode() != adapter()->hashCode())
					{
						auto temp = adapter()->toData(adp->toAnyValues(pdata));
						SchemaType::callInnerProcess(_innerProcess, ct, temp);
					}
					else
					{
						SchemaType::callInnerProcess(_innerProcess, ct, pdata);
					}
				}
			}
		protected:
			InnerProcessType _innerProcess;
		};
	}
}
