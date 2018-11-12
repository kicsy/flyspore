#include "Context.h"
#include "Session.h"
#include "Spore.h"
#include "Data.h"
fs::L0::Context::Context(P_Spore ps, P_Session pss, P_AnyValues plocal):
	_ps(ps)
	,_pss(pss)
	,_plocal(plocal)
	,local(*plocal.get())
	,session(pss->values())
{
}

fs::L0::Context::~Context()
{
}

bool fs::L0::Context::push(const std::string &outPinName, const P_Data &ppack)
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

fs::L0::P_Spore fs::L0::Context::spore()
{
	return _ps;
}
