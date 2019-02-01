#include <algorithm>
#include "Pin.h"
#include "DefaultNest.h"
#include "Session.h"
#include "Context.h"
#include "Path.h"
#include "../Level0/task_pool.h"
using namespace fs::L0;

namespace fs
{
	namespace L1
	{
		Pin::Pin(const std::weak_ptr<DefaultNest>& pNest, Pin_Type type):
			BasicNodeMap(pNest),
			_type(type)
		{
			auto _op = opr();
			if (_op)
			{
				set(_op->create(std::string("pin")), std::string("class"));
				std::string strType;
				switch (type)
				{
				case fs::L1::Pin_Type::IN_PIN:
					strType = "in";
					break;
				case fs::L1::Pin_Type::OUT_PIN:
					strType = "out";
					break;
				default:
					strType = "unknown";
					break;
				}
				set(_op->create(strType), std::string("pin_type"));
			}
		}

		Pin::~Pin()
		{
		}

		bool Pin::push(const const std::shared_ptr<Data> &data)
		{
			if (!data)
			{
				return false;
			}
			P_Session pss = data->getSession();
			if (!pss || pss->status() == Session_Status::BlackOut)
			{
				return false;
			}
			if (_type == Pin_Type::IN_PIN && enableProcess())
			{
				pss->increaseTask();
				task_pool::get_instance().submit(&Pin::task_process, this, data);
				return true;
			}
			else if (_type == Pin_Type::OUT_PIN)
			{
				std::shared_lock<std::shared_mutex> lock(_mutex);
				for (auto &path : _outPaths)
				{
					if (path != nullptr && path->isvalid()) {
						path->move(data);
					}
				}
			}
		}

		void Pin::task_process(const P_Data &data)
		{
			P_Session pss = data->getSession();
			Context cc(pss);
			process(cc, data);
			pss->decreaseTask();
		}

		P_DataAdapter Pin::adapter() const
		{
			return NULL;
		}

		Pin_Type Pin::type() const
		{
			return _type;
		}

		std::vector<P_Path> Pin::paths() const
		{
			std::shared_lock<std::shared_mutex> lock(_mutex);
			return _outPaths;
		}

		bool Pin::addPath(P_Path path)
		{
			if (!path)
			{
				return false;
			}
			std::unique_lock<std::shared_mutex> lock(_mutex);
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
			std::unique_lock<std::shared_mutex> lock(_mutex);
			if (path->from().get() == this)
			{
				isok = std::remove_if(_outPaths.begin(), _outPaths.end(), [&](P_Path& pp)-> bool {
					isok = true;
					return pp == path;
				}) != _outPaths.end();
			}
			else if (path->to().get() == this)
			{
				isok = std::remove_if(_inPaths.begin(), _inPaths.end(), [&](P_Path& pp)->bool {
					isok = true;
					return pp == path;
				}) != _inPaths.end();
			}
			return isok;
		}
	}
}

