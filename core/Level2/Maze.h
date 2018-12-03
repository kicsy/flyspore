#pragma once
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
		protected:
			P_Session _session_call;
			mutable std::mutex _mut_console;

		};
	}
}

