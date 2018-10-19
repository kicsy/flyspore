#include "Context.h"
#include "Session.h"
#include "Spore.h"
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

fs::P_Data fs::Context::newData()
{
	if (_pss)
	{
		return _pss->newData();
	}
	return P_Data();
}

bool fs::Context::push(const std::string &outPinName, P_Data ppack)
{
	if (!_ps)
	{
		return false;
	}
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
