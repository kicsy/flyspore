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
			void _out_to_console(Context& cc, DataPack& data);
			void _error_to_console(Context& cc, DataPack& data);
		protected:
			P_Session _session_call;
			mutable std::mutex _mut_console;

		};
	}
}

