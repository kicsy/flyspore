#pragma once
#include "Statement.h"
namespace fs
{
	namespace L0
	{
		class Data
		{
		public:
			Data(const P_Session& session = NULL) :_session(session) {}
			virtual ~Data() {}
			virtual P_Session  getSession() const { return _session; }
		protected:
			void setSession(const P_Session &session) { _session = session; }
		protected:
			P_Session _session;
			friend Context;
			friend Session;
		};
	}
}