#include "Schema.h"
#include "DataPack.h"
namespace fs
{
	std::shared_ptr<Schema< defaultProcess>> __defaultSchema = std::make_shared<Schema< defaultProcess>>(
		([](std::function<defaultProcess> innerProcess, Context& context, const P_Data& pdata) {
			DataPack* pvs = (DataPack*)pdata.get();
			if (innerProcess && pvs)
			{
				innerProcess(context, *pvs);
			}
		}),
		([](const AnyValues &values) -> P_Data {
			return std::move(std::make_shared<DataPack>(values));
		}),
		([](const P_Data& pdata) ->AnyValues
		{
			DataPack* p = (DataPack*)pdata.get();
			if (p)
			{
				return *(AnyValues*)p;
			}
			return std::move(AnyValues());
		}));

	std::shared_ptr<fs::Schema< fs::defaultProcess>> defaultSchema()
	{
		return __defaultSchema;
	}
}
