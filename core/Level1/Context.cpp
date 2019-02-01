#include "Context.h"
#include "Session.h"
#include "Data.h"
fs::L1::Context::Context(P_Session pss):
	_pss(pss)
	,session(pss->values())
{
}

fs::L1::Context::~Context()
{
}

bool fs::L1::Context::push(const std::string &outPinName, const P_Data &ppack)
{
	if (!_ps || !ppack)
	{
		return false;
	}
	ppack->setSession(_pss);
	P_Pin outPin = _ps->getPin(outPinName);
	if (outPin && outPin->type() == Pin_Type::OUT_PIN)
	{
		outPin->push(ppack);
		return true;
	}
	return false;
}

fs::L1::P_Spore fs::L1::Context::spore()
{
	return _ps;
}
