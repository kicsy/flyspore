#include "Path.h"
#include "Pin.h"

bool fs::Path::move(P_Data data)
{
	if (_outPin)
	{
		_outPin->push(data);
		return true;
	}
	return false;
}

fs::Path::~Path()
{

}

std::shared_ptr<fs::Pin> fs::Path::inPin() const 
{
	return _inPin;
}

std::shared_ptr<fs::Pin> fs::Path::outPin() const
{
	return _outPin;
}

bool fs::Path::isvalid() const
{
	return _inPin != nullptr && _outPin != nullptr;
}
