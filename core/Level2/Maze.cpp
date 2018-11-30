#include <iostream>
#include <iomanip>
#include "Maze.h"
#include "ConsoleColor.h"

fs::L2::Maze::Maze()
{
	P_Spore _root = Spore::newSpore("/");
	P_Spore _spore = _root->addChild("Maze");
	defaultSchema().addPin(_spore, "call",
		std::bind(&Maze::_call, this, std::placeholders::_1, std::placeholders::_2));
	defaultSchema().addPin(_spore, "out");
	defaultSchema().addPin(_spore, "error");

	P_Spore _console = _root->addChild("Console");
	defaultSchema().addPin(_console, "out",
		std::bind(&Maze::_echo_out, this, std::placeholders::_1, std::placeholders::_2));
	defaultSchema().addPin(_console, "error",
		std::bind(&Maze::_echo_error, this, std::placeholders::_1, std::placeholders::_2));

	Path::connect(_spore, "out", _console, "out", "out");
	Path::connect(_spore, "error", _console, "error", "error");

	_session_call = Session::newSession(_spore->getPin("call"));

}

fs::L2::Maze::~Maze()
{

}

void fs::L2::Maze::call(const std::string & cmd)
{
	if (_session_call)
	{
		DataPack cmdData;
		cmdData.ref<std::string>("cmd") = cmd;
		_session_call->submit(cmdData);
	}
}

void fs::L2::Maze::run()
{
	_echo_about();
	std::string cmd;
	do 
	{
		_read_cmd(cmd);
		if (cmd.length() > 0)
		{
			call(cmd);
		}
	} while (cmd != "quit");
}

void fs::L2::Maze::_call(Context& cc, DataPack& data)
{
	std::string &cmd = data.ref<std::string>("cmd");

	auto echo = std::make_shared<DataPack>();
	echo->ref<std::string>("echo") = cmd;

	if (cmd == "info")
	{

	}

	cc.push("out", echo);
	cc.push("error", echo);
}

void fs::L2::Maze::_read_cmd(std::string &instring)
{
	std::getline(std::cin, instring);
}

void fs::L2::Maze::_echo_out(Context& cc, DataPack& data)
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::string &echo = data.ref<std::string>("echo");
	std::cout << "\r";
	std::cout.flush();
	std::cout << echo << std::endl << INFLAG;
	std::cout.flush();
}

void fs::L2::Maze::_echo_error(Context& cc, DataPack& data)
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::string &echo = data.ref<std::string>("echo");
	std::cerr << "\r";
	std::cerr.flush();
	std::cerr << RED << echo << RESET << std::endl << INFLAG;
	std::cout.flush();
}

void fs::L2::Maze::_echo_about()
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::cout << "\r";
	std::cout.flush();
	std::cout << GREEN << "Maze 0.0.6.0 (fly spore version 0.0.6.0)\n" << RESET << INFLAG;
	std::cout.flush();
}
