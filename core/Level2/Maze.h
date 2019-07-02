#pragma once
#include <string>
#include "../Level1/Level1.h"

using namespace fs::L1;
namespace fs
{
	namespace L2
	{
		class Maze
		{
		public:
			Maze();
			~Maze();

			void call(const std::string & cmd);
			void run();
		protected:
			void _call(Context& cc, DataPack& data);
			void _read_cmd(std::string &instring);
			void _out(Context& cc, DataPack& data);
			void _error(Context& cc, DataPack& data);
			void _echo_textBlock(std::ostream &stream, std::string &textBlock);
			void _echo_Welcome();
			void buildCallMap();
			void _call_load(AnyValues& session, std::string &cmd);
			void _call_quit(AnyValues& session, std::string &cmd);
		protected:
			std::shared_ptr<DefaultNest> _nest;
			std::shared_ptr<Session> _session_call;
			mutable std::mutex _mut_console;
			using CallFuntion = std::function< void(AnyValues&, std::string&) >;
			std::map<std::string, CallFuntion> _callMap;

		};
	}
}

