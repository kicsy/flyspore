#pragma once
#include "AnyValues.h"
#include <shared_mutex>
namespace fs
{
	class SyncValueSet : public AnyValues
	{
	public:

	protected:
		std::shared_mutex _mutex;
	};
}

