#include <algorithm>
#include "Pin.h"
#include "Spore.h"
#include "Path.h"
namespace fs
{
	Pin::Pin(PW_Spore spore, std::string name, Pin_Type type):
		_spore(spore)
		, _type(type)
		, _name(name)
	{

	}

	Pin::~Pin()
	{
	}

	void Pin::push(const P_Data &data)
	{
		P_Spore spore = _spore.lock();
		if (spore == nullptr)
			return;
		if (_type == Pin_Type::IN_PIN){
			spore->input(shared_from_this(), data);
		}
		std::shared_lock<std::shared_mutex> lock(_paths_mutex);
		for (auto &path : _outPaths){
			if (path != nullptr && path->isvalid()){
				path->move(data);
			}
		}
	}

	fs::P_DataAdapter Pin::adapter() const 
	{
		return NULL;
	}

	Pin_Type Pin::type() const
	{
		return _type;
	}

	std::vector<P_Path> Pin::paths() const 
	{
		std::shared_lock<std::shared_mutex> lock(_paths_mutex);
		return _outPaths;
	}

	bool Pin::addPath(P_Path path)
	{
		if (!path)
		{
			return false;
		}
		std::unique_lock<std::shared_mutex> lock(_paths_mutex);
		if (path->from().get() == this)
		{
			auto iter = std::find(_outPaths.begin(), _outPaths.end(), path);
			if (iter == _outPaths.end())
			{
				_outPaths.push_back(path);
			}
			return true;
		}
		else if (path->to().get() == this)
		{
			auto iter = std::find(_inPaths.begin(), _inPaths.end(), path);
			if (iter == _inPaths.end())
			{
				_inPaths.push_back(path);
			}
			return true;
		}
		return false;
	}

	bool Pin::removePath(P_Path path)
	{
		if (!path)
		{
			return false;
		}
		bool isok = false;
		std::unique_lock<std::shared_mutex> lock(_paths_mutex);
		if (path->from().get() == this)
		{
			std::remove_if(_outPaths.begin(), _outPaths.end(), [&](P_Path& pp) {
				isok = true;
				return pp == path;
			});
		}
		else if (path->to().get() == this)
		{
			std::remove_if(_inPaths.begin(), _inPaths.end(), [&](P_Path& pp) {
				isok = true;
				return pp == path;
			});
		}
		return isok;
	}

	fs::P_Spore Pin::spore() const
	{
		return _spore.lock();
	}

	std::string Pin::name() const 
	{
		return _name;
	}
}

