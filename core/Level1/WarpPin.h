#pragma once
#include "Pin.h"
namespace fs
{
	namespace L1
	{
		template< typename SchemaType>
		class WarpPin : public Pin
		{
		public:
			using InnerProcessType = typename SchemaType::InnerProcessType;
			WarpPin(PW_Spore spore, std::string name, Pin_Type type, std::shared_ptr< SchemaType> schema, InnerProcessType innerProcess) :
				Pin(spore, name, type)
				, _schema(schema)
				, _innerProcess(innerProcess)
			{
			}
			virtual P_DataAdapter adapter() const
			{
				return std::move(std::dynamic_pointer_cast<DataAdapter>(_schema));
			}
		protected:
			virtual bool enableProcess() const { return NULL != _innerProcess; }
			virtual void process(Context& ct, const P_Data& pdata)
			{
				if (_innerProcess)
				{
					_schema->callInnerProcess(_innerProcess, ct, pdata);
				}
			}
		protected:
			std::shared_ptr< SchemaType> _schema;
			InnerProcessType _innerProcess;
		};
	}
}
