#pragma once
#include <memory>
#include "BasicNodeMap.h"
namespace fs
{
	namespace L1
	{
		class DefaultNest;
		class Pin;
		class Data;
		class Path : public BasicNodeMap
		{
		public:
			~Path();
			Path(const Path&) = delete;
			Path& operator=(const Path&) = delete;
			P_Pin from() const;
			P_Pin to() const;
			bool isvalid() const;
			bool move(const std::shared_ptr<Data>& data);
			static P_Path connect(P_Pin &from, P_Pin &to, const std::string &name = "");
			static P_Path connect(P_Spore &fromSpore, const std::string &fromPinName,
				P_Spore &toSpore, const std::string &toPinName, const std::string &name = "");
			static bool release(const P_Path &path);
		protected:
			Path(const std::weak_ptr<DefaultNest>& pNest, const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to);
			bool _build();
			bool _release();
		protected:
			std::string _name;
			P_Pin _from;
			P_Pin _to;
			friend class DefaultNest;
		};
	}
}


