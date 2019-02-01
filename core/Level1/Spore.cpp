/*!
 * \class classname
 *
 * \ingroup GroupName
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note 
 *
 * \author kicsy
 *
 * \version 1.0
 *
 * \date 2016/08/19 12:38
 *
 * Contact: lmj07luck@126.com
 *
 */
#include "Spore.h"
#include "Pin.h"
#include "Path.h"
#include "../Level0/task_pool.h"
#include "Session.h"
#include "Context.h"
#include "DataPack.h"
#include "DefaultNest.h"
using namespace fs::L0;

namespace fs
{
	namespace L1
	{
		Spore::Spore(const std::weak_ptr<DefaultNest>& pNest):
			BasicNodeMap(pNest)
		{
			auto _op = opr();
			if (_op)
			{
				set(_op->createMap(), std::string("/"));
				set(_op->createMap(), std::string("pins"));
				set(_op->createMap(), std::string("paths"));
			}
		}

		Spore::~Spore()
		{
		}


		P_Path Spore::create_or_find_Path(P_Pin from, P_Pin to, const std::string &name /*= ""*/)
		{
			std::unique_lock<std::shared_mutex> lock(_paths_mutex);
			auto iter = std::find_if(_paths.begin(), _paths.end(), [&](P_Path& pp) {
				return pp->from() == from && pp->to() == to;
			});
			if (iter != _paths.end())
			{
				return *iter;
			}
			P_Path path = std::make_shared<Path>(from, to, name);
			path->_build();
			_paths.push_back(path);
			return std::move(path);
		}

		bool Spore::deletePath(const P_Path &path)
		{
			bool isOk = false;
			std::unique_lock<std::shared_mutex> lock(_paths_mutex);
			isOk = std::remove_if(_paths.begin(), _paths.end(), [&](P_Path& pp)->bool {
				if (pp == path)
				{
					pp->_release();
					return true;
				}
				return false;
			}) != _paths.end();
			return isOk;
		}

	}
}


