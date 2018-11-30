#pragma once
#ifdef _WIN32
#include <windows.h>
struct ConsoleColor {
	ConsoleColor(WORD attribute) :_color(attribute) {};
	WORD _color;
};

template <class _Elem, class _Traits>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& i, ConsoleColor& c)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, c._color);
	return i;
}

#define RESET   ConsoleColor(FOREGROUND_INTENSITY)
#define BLACK   ConsoleColor(FOREGROUND_INTENSITY)
#define RED     ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_RED)
#define GREEN   ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_GREEN)
#define YELLOW  ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE)
#define BLUE    ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_BLUE)
#define MAGENTA ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE)
#define CYAN    ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE)
#define WHITE   ConsoleColor(FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)
#define BOLDBLACK   ConsoleColor(BACKGROUND_INTENSITY)
#define BOLDRED     ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_RED)
#define BOLDGREEN   ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_GREEN)
#define BOLDYELLOW  ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE)
#define BOLDBLUE    ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_BLUE)
#define BOLDMAGENTA ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE)
#define BOLDCYAN    ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE)
#define BOLDWHITE   ConsoleColor(BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE)

#else
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#endif

