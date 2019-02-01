/*!
 * \file Spore.h
 * \date 2016/08/19 12:38
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Statement.h"
#include "BasicNodeMap.h"
#include "Schema.h"
#include "WarpPin.h"

namespace fs
{
	namespace L1
	{
		class DefaultNest;
		class Spore : protected BasicNodeMap
		{
		public:
			~Spore();
		protected:
			Spore(const std::weak_ptr<DefaultNest>& pNest);
			Spore(const Spore&) = delete;
			P_Path create_or_find_Path(P_Pin from, P_Pin to, const std::string &name = "");
			bool deletePath(const P_Path &path);
		protected:
			friend class DefaultNest;
			friend class Session;
			friend class Path;
		};
	}
}


