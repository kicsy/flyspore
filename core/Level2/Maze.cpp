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
		std::bind(&Maze::_out, this, std::placeholders::_1, std::placeholders::_2));
	defaultSchema().addPin(_console, "error",
		std::bind(&Maze::_error, this, std::placeholders::_1, std::placeholders::_2));

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
		cmdData.set<std::string>("cmd", cmd);
		_session_call->submit(cmdData);
	}
}

void fs::L2::Maze::run()
{
	_echo_Welcome();
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
	std::string  cmd = data.get<std::string>("cmd");
	auto echo = std::make_shared<DataPack>();
	echo->set<std::string>("echo", cmd);

	if (cmd == "info")
	{

	}

	cc.push("out", echo);
}

void fs::L2::Maze::_read_cmd(std::string &instring)
{
	std::getline(std::cin, instring);
}

void fs::L2::Maze::_out(Context& cc, DataPack& data)
{
	std::string echo = data.get<std::string>("echo");
	_echo_textBlock(std::cout, echo);
}

void fs::L2::Maze::_error(Context& cc, DataPack& data)
{
	std::string echo = data.get<std::string>("echo");
	_echo_textBlock(std::cerr, echo);
}

void fs::L2::Maze::_echo_textBlock(std::ostream &stream, std::string &textBlock)
{
	std::unique_lock<std::mutex> lk(_mut_console);
	std::cout << "\r" << CYAN << "FlySpore  " << RESET << "/home" << "\n"  ;

	if (&stream == &std::cerr)
	{
		stream <<  RED << textBlock << RESET << std::endl << INFLAG;
	}
	else
	{
		stream <<  textBlock << std::endl << INFLAG;
	}
}

void fs::L2::Maze::_echo_Welcome()
{
	std::cout << "\r" << GREEN << "FlySpore"<< RESET << "  (version 0.0.6.0)\n"  << INFLAG;
}
