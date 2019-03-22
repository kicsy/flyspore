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
            DefaultWarpPin(const std::string& name, Pin_Type type, ProcessType innerProcess):
            WarpPin<DataPack>(name, type, innerProcess)
            {
            }
        };
    }
}