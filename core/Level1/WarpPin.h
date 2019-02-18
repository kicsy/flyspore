#pragma once
#include "Pin.h"
namespace fs
{
	namespace L1
	{
		class DataAdapter;
		class Data;
		template< typename SchemaType>
		class WarpPin : public Pin, public SchemaType
		{
		public:
			using InnerProcessType = typename SchemaType::InnerProcessType;
			WarpPin(const std::string& name, Pin_Type type, InnerProcessType innerProcess) :
				Pin(name, type)
				, _innerProcess(innerProcess)
			{
			}
			virtual std::shared_ptr<DataAdapter> adapter()
			{
				auto _p = std::dynamic_pointer_cast<WarpPin<SchemaType>>(shared_from_this());
				return std::dynamic_pointer_cast<DataAdapter>(_p);
			}
		protected:
			virtual bool enableProcess() const { return NULL != _innerProcess; }
			virtual void process(Context& ct, const std::shared_ptr<Data>& pdata)
			{
				if (_innerProcess)
				{
					SchemaType::callInnerProcess(_innerProcess, ct, pdata);
				}
			}
		protected:
			InnerProcessType _innerProcess;
		};
	}
}
