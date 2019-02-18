#pragma once
#include <memory>
namespace fs
{
	namespace L1
	{
		class Session;
		class Data
		{
		public:
			Data(const std::shared_ptr<Session>& session = NULL) :_session(session) {}
			virtual ~Data() {}
			virtual std::shared_ptr<Session>  getSession() const { return _session; }
			virtual unsigned int hashCode() = 0;
		protected:
			void setSession(const std::shared_ptr<Session>& session) { _session = session; }
		protected:
			std::shared_ptr<Session> _session;
			friend class Context;
			friend class Session;
		};
	}
}