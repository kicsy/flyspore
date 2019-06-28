#include "DefaultWarpPin.h"
#include "DefaultNest.h"

namespace fs
{
	namespace L1
	{

        DefaultWarpPin::DefaultWarpPin(const std::weak_ptr<DefaultNest>& pNest, const std::string& name, Pin_Type type, ProcessType innerProcess):
        WarpPin<DataPack>(pNest, name, type, innerProcess)
        {
        }
        
        std::shared_ptr<Pin> DefaultWarpPin::clone(const std::string& newName/*= std::string("")*/)
        {
            auto newPin = nest()->createPin(
                newName.empty() ? name() : newName,
                type(),
                _innerProcess
            );
            return newPin;
        }
    }
}
