#include <algorithm>
#include "Pin.h"
#include "Spore.h"
#include "Path.h"
namespace fs
{
	Pin::Pin(PW_Spore spore, Pin_Type type):
		_spore(spore)
		,_type(type)
	{
	}

	Pin::Pin(PW_Spore spore, Pin_Type type, Pin_Process process) :
		_spore(spore)
		, _type(type)
		, _process(process)
	{
	}

	Pin::~Pin()
	{
	}

	void Pin::push(P_Data data)
	{
		P_Spore spore = _spore.lock();
		if (spore == nullptr)
			return;
		if (_type == Pin_Type::IN_PIN){
			spore->input(shared_from_this(), data);
		}
		std::shared_lock<std::shared_mutex> lock(_paths_mutex);
		for (auto &path : _paths){
			if (path != nullptr && path->isvalid()){
				path->move(data);
			}
		}
	}

	Pin_Type Pin::type() const
	{
		return _type;
	}

	std::vector<P_Path> Pin::paths()
	{
		std::shared_lock<std::shared_mutex> lock(_paths_mutex);
		return _paths;
	}

	bool Pin::addPath(P_Path path)
	{
		std::unique_lock<std::shared_mutex> lock(_paths_mutex);
		_paths.push_back(path);
		return true;
	}

	fs::P_Spore Pin::spore()
	{
		return _spore.lock();
	}

}

