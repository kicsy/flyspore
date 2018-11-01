#include "Context.h"
#include "Session.h"
#include "Spore.h"
#include "Data.h"
fs::Context::Context(P_Spore ps, P_Session pss, P_AnyValues plocal):
	_ps(ps)
	,_pss(pss)
	,_plocal(plocal)
	,local(*plocal.get())
	,session(pss->values())
{
}

fs::Context::~Context()
{
}

bool fs::Context::push(const std::string &outPinName, const P_Data &ppack)
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

fs::P_Spore fs::Context::spore()
{
	return _ps;
}
