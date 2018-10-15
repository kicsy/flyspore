#pragma once
#include <memory>
#include "Statement.h"
namespace fs
{
	class Path
	{
	public:
		~Path();
		Path(const Path&) = delete;
		Path& operator=(const Path&) = delete;
		P_Pin from() const;
		P_Pin to() const ;
		bool isvalid() const;
		bool move(P_Data data);
		static P_Path connect(P_Pin from, P_Pin to);
	protected:
		P_Pin _from;
		P_Pin _to;
		friend class Spore;
	};
}


