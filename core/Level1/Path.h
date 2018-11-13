#pragma once
#include <memory>
#include "Statement.h"
namespace fs
{
	namespace L1
	{
		class Path : public std::enable_shared_from_this<Path>
		{
		public:
			Path(P_Pin from, P_Pin to, const std::string &name = "");
			~Path();
			Path(const Path&) = delete;
			Path& operator=(const Path&) = delete;
			P_Pin from() const;
			P_Pin to() const;
			bool isvalid() const;
			bool move(P_Data data);
			static P_Path connect(P_Pin &from, P_Pin &to, const std::string &name = "");
			static bool release(const P_Path &path);
		protected:
			bool _build();
			bool _release();
		protected:
			std::string _name;
			P_Pin _from;
			P_Pin _to;
			friend class Spore;
		};
	}
}


