#pragma once
#include <memory>
#include "AnyValues.h"
namespace fs
{
	namespace L1
	{
		class Session;
		class Spore;
		class Data;
		class Context
		{
		public:
			Context(const std::shared_ptr<Spore>& spore, const std::shared_ptr<Session>& pss);
			~Context();
			AnyValues& session;
			bool push(const std::string  &outPinName, const std::shared_ptr<Data> &ppack);
			std::shared_ptr<Spore> spore();
		protected:
			std::shared_ptr<Session> _pss;
			std::shared_ptr<Spore> _spore;
		};
	}
}


