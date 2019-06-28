#pragma once
#include "WarpPin.h"
#include "DataPack.h"

namespace fs
{
	namespace L1
	{
        class DefaultWarpPin : public WarpPin<DataPack>
        {
        public:
            DefaultWarpPin(const std::weak_ptr<DefaultNest>& pNest, const std::string& name, Pin_Type type, ProcessType innerProcess);
            std::shared_ptr<Pin> clone(const std::string& newName/*= std::string("")*/) override;

        };
    }
}
