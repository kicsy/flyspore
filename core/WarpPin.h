#pragma once
#include "Pin.h"
namespace fs
{
	template< typename SchemaType>
	class WarpPin : public Pin
	{
	public:
		using InnerProcessType = typename SchemaType::InnerProcessType;
	protected:
		WarpPin(PW_Spore spore, std::string name, Pin_Type type, InnerProcessType innerProcess) :
			Pin(spore, name, type)
			, _innerProcess(innerProcess)
		{
			//DataAdapter *p = SchemaType
		}
		virtual void process(Context& ct, P_Data& pdata)
		{
			if (_innerProcess)
			{
				_schema.callInnerProcess(_innerProcess, ct, pdata);
			}
		}
		virtual bool enableProcess() { return NULL != _innerProcess; }
	protected:
		SchemaType _schema;
		InnerProcessType _innerProcess;
		friend class Spore;
	};

}
