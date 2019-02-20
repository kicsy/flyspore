#pragma once
#include "Data.h"
#include "AnyValues.h"
#include "DataAdapter.h"
namespace fs
{
	namespace L1
	{
		class DataPackAdapter;
		class DataPack : public BasicData<DataPackAdapter>, public AnyValues
		{
		public:
			DataPack(const AnyValues &any = AnyValues()) :
				BasicData<DataPackAdapter>()
				, AnyValues(any) {
			}
			virtual ~DataPack() {
			}
			DataPack(const DataPack&) = delete;
			DataPack& operator=(const DataPack&) = delete;
		};

		class DataPackAdapter : public DataAdapter
		{
		public:
			virtual unsigned int hashCode() const override
			{
				return *(const unsigned int*)("DataPackAdapter");
			}

			virtual std::shared_ptr<Data> toData(const AnyValues &values) const override
			{
				auto mypack = new DataPack(values);
				return std::shared_ptr<Data>(mypack);

			}
			virtual AnyValues toAnyValues(const std::shared_ptr<Data>& pdata) const override
			{
				auto *dataPack = (const DataPack*)pdata.get();
				return *(AnyValues*)dataPack;
			}
		};
	}
}