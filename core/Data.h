#pragma once
#include "Statement.h"
namespace fs
{
	class Data
	{
	public:
		Data(const P_Session& session):_session(session) {}
		virtual ~Data() {}
		virtual P_Session  getSession() { return _session; }
	protected:
		void setSession(P_Session session) { _session = session; }
	protected:
		P_Session _session;
		friend Context;
		friend Session;
	};
}