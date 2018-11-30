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
		std::bind(&Maze::_out_to_console, this, std::placeholders::_1, std::placeholders::_2));
	defaultSchema().addPin(_console, "error",
		std::bind(&Maze::_error_to_console, this, std::placeholders::_1, std::placeholders::_2));

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
	std::cout <<GREEN << "Maze 0.0.6.0 (fly spore version 0.0.6.0)\n" << RESET;
	std::string cmd;
	do 
	{
		std::cin >> cmd;
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
int __count = 0;
void fs::L2::Maze::_out_to_console(Context& cc, DataPack& data)
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::string &echo = data.ref<std::string>("echo");
	std::cout << std::setw(8) << __count++;
	std::cout << echo << std::endl;
}

void fs::L2::Maze::_error_to_console(Context& cc, DataPack& data)
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::string &echo = data.ref<std::string>("echo");
	std::cout << std::setw(8) << __count++;
	std::cerr << RED << echo <<RESET << std::endl;
}
