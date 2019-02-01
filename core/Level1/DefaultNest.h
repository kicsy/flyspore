#pragma once
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{
		class Spore;
		class Pin;
		class Path;
		class DefaultNest : public BasicNodeOperator
		{
		public:
			DefaultNest();
			~DefaultNest();

			virtual std::shared_ptr<Spore> createSpore();
			virtual std::shared_ptr<Pin>	createPin();
			virtual std::shared_ptr<Path>	connectPin(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string& name);
		public:
			std::shared_ptr<Spore> getParent(std::shared_ptr<Pin> spore);
			std::shared_ptr<Spore> getParent(std::shared_ptr<Spore> pin);
		};

	}
}
