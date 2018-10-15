#include "Context.h"
#include "Session.h"
#include "Spore.h"
fs::Context::Context(P_Spore ps, P_Session pss, P_AnyValues plocal):
	_ps(ps)
	,_pss(pss)
	,_plocal(plocal)
	,local(*plocal.get())
	,session(pss->propertis())
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

fs::P_Data fs::Context::newData(P_Data && pdata)
{
	if (_pss)
	{
		return _pss->newData(*pdata);
	}
	return P_Data();
}

bool fs::Context::push(const std::string &outPinName, P_Data ppack)
{
	if (!_ps)
	{
		return false;
	}
	P_Pin outPin = _ps->getPin(std::move(outPinName), Pin_Type::OUT_PIN);
	if (outPin)
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
