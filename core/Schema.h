#pragma once
#include <cassert>
#include "Pin.h"
#include "Statement.h"
#include "DataAdapter.h"
namespace fs
{
	template<typename ProcessType /*= void(Context&, DataPack&)*/>
	class Schema : public DataAdapter, public std::enable_shared_from_this<Schema<ProcessType>>
	{
	public:
		using InnerProcessType = std::function<typename ProcessType>;
		using TransformFunc = std::function<void(InnerProcessType innerProcess, Context& context, const P_Data& pdata)>;
		using ToDataFunc = std::function<P_Data(const AnyValues &values)>;
		using ToAnyValuesFunc = std::function<AnyValues(const P_Data& pdata)>;

		explicit Schema(TransformFunc transformFunc = NULL,
			ToDataFunc toDataFunc = NULL,
			ToAnyValuesFunc toAnyValuesFunc = NULL) 
			:std::enable_shared_from_this<Schema<ProcessType>>()
			,_transformFunc(transformFunc)
			,_toDataFunc(toDataFunc)
			,_toAnyValuesFunc(toAnyValuesFunc)
		{
			//if (!_transformFunc) _transformFunc = [](InnerProcessType innerProcess, Context& context, const P_Data& pdata) {
			//	//transform...
			//	DataPack* pvs = (DataPack*)pdata.get();
			//	if (innerProcess && pvs)
			//	{
			//		innerProcess(context, *pvs);
			//	}
			//};
			//if (!_toDataFunc) _toDataFunc = [](const AnyValues &values) -> P_Data {
			//	return std::move(std::make_shared<DataPack>(values));
			//};
			//if (!_toAnyValuesFunc) _toAnyValuesFunc = [](const P_Data& pdata) ->AnyValues
			//{
			//	DataPack* p = (DataPack*)pdata.get();
			//	if (p)
			//	{
			//		return *(AnyValues*)p;
			//	}
			//	return std::move(AnyValues());
			//};
		}
		virtual ~Schema() {}

		virtual void callInnerProcess(InnerProcessType innerProcess, Context& context, const P_Data& pdata)
		{
			if (_transformFunc)
				_transformFunc(innerProcess, context, pdata);
		}

		P_Pin addPin(P_Spore &spore, const std::string &name, InnerProcessType process = nullptr)
		{
			return addPin(
				spore,
				name,
				process == nullptr ? Pin_Type::OUT_PIN : Pin_Type::IN_PIN,
				process);
		}

		P_Pin addPin(P_Spore &spore, const std::string &name, Pin_Type type, InnerProcessType process = nullptr)
		{
			auto pin = P_Pin(new WarpPin<Schema<typename ProcessType>>(spore, name, type, shared_from_this(), process));
			return spore->addPin(pin);
		}

		virtual bool equal(DataAdapter* other)
		{
			return (DataAdapter*)this == other;
		}
		virtual P_Data toData(const AnyValues &values)
		{
			if (_toDataFunc)
				return _toDataFunc(values);
			return NULL;
		}
		virtual AnyValues toAnyValues(const P_Data& pdata)
		{
			if (_toAnyValuesFunc)
				return _toAnyValuesFunc(pdata);
			return std::move(AnyValues());
		}
	protected:
		friend Spore;
		TransformFunc _transformFunc;
		ToDataFunc _toDataFunc;
		ToAnyValuesFunc _toAnyValuesFunc;
	};

	
	using defaultProcess = void(Context&, DataPack&);
	std::shared_ptr<Schema< defaultProcess>> defaultSchema();
}

