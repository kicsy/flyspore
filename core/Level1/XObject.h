#pragma once
#include <memory>
class XObject :public  std::enable_shared_from_this<Spore>
{
public:
	XObject();
	~XObject();
};

