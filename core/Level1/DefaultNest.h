#pragma once
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{
		class DefaultNest : public BasicNodeOperator
		{
		public:
			DefaultNest();
			~DefaultNest();

			virtual P_Spore createSpore();
			virtual P_Pin	createPin();
		};

	}
}
