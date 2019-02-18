#include "Context.h"
#include "Session.h"
#include "Data.h"
#include "Spore.h"
namespace fs
{
	namespace L1
	{

		Context::Context(const std::shared_ptr<Spore>& spore, const std::shared_ptr<Session>& pss):
			_spore(spore)
			,_pss(pss)
			, session(pss->values())
		{
		}

		fs::L1::Context::~Context()
		{
		}

		bool Context::push(const std::string &outPinName, const std::shared_ptr<Data> &ppack)
		{
			if (!ppack)
			{
				return false;
			}
			ppack->setSession(_pss);
			std::shared_ptr<Pin> outPin = _spore->getPin(outPinName);
			if (outPin && outPin->type() == Pin_Type::OUT_PIN)
			{
				outPin->push(ppack);
				return true;
			}
			return false;
		}

		std::shared_ptr<Spore> Context::spore()
		{
			return _spore;
		}
	}
}
