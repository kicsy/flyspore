#pragma once
#include <memory>
namespace fs
{
	namespace L1
	{
		class Session;
		class DataAdapter;
		class Data
		{
		public:
			Data() {}
			virtual ~Data() {}
			virtual std::shared_ptr<Session>  getSession() const { return _session; }
			virtual DataAdapter* adapter() const =0;
		protected:
			void setSession(const std::shared_ptr<Session>& session) { _session = session; }
		protected:
			std::shared_ptr<Session> _session;
			friend class Context;
			friend class Session;
		};

		template< typename AdapterType>
		class BasicData : public Data
		{
		public:
			virtual DataAdapter* adapter() const
			{
				return adapterInst();
			}
			static DataAdapter* adapterInst()
			{
				static AdapterType adp;
				return &adp;
			}
		};
	}
}
