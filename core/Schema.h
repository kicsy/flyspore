#pragma once
#include <cassert>
#include "Pin.h"
#include "Statement.h"
#include "DataPack.h"
#include "DataAdapter.h"
namespace fs
{
	template<typename ProcessType>
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
			,_toAnyValuesFunc(toAnyValuesFunc){}
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



	template<typename DataSetType>
	class LightSchema : public DataAdapter, public std::enable_shared_from_this<LightSchema<DataSetType>>
	{
	public:
		using InnerProcessType = std::function< void(Context&, typename DataSetType&)>;
		explicit LightSchema():std::enable_shared_from_this<Schema<ProcessType>>()
		virtual ~LightSchema() {}
		virtual void callInnerProcess(InnerProcessType innerProcess, Context& context, const P_Data& pdata)
		{
			DataSetType* pvs = (DataSetType*)pdata.get();
			if (innerProcess && pvs)
			{
				innerProcess(context, *pvs);
			}
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

			return P_Data((Data*)(toDataFunc(values)));
		}

		virtual AnyValues toAnyValues(const P_Data& pdata)
		{
			DataSetType* p = (DataSetType*)pdata.get();
			if (p)
				return toAnyValuesFunc(*p);
			return std::move(AnyValues());
		}
	protected:
		virtual DataSetType* toDataFunc(const AnyValues &)
		{
			return NULL;
		}

		virtual AnyValues toAnyValuesFunc(const DataSetType &)
		{
			return std::move(AnyValues());
		}

	protected:
		friend Spore;
	};

	class DefaultSchema : public LightSchema<DataPack>
	{
	protected:
		virtual DataPack* toDataFunc(const AnyValues & values)
		{
			return new DataPack(values);
		}

		virtual AnyValues toAnyValuesFunc(const DataPack & dataPack)
		{
			return *(AnyValues*)(&dataPack);
		}
	};
}

