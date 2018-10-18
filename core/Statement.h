#pragma once
#include <memory>
#include <functional>
#include <string>
namespace fs
{
	using IdType = unsigned long long;

	class AnyValues;
	using P_AnyValues = std::shared_ptr<AnyValues>;

	class DataPack;
	using P_Data = std::shared_ptr<DataPack>;

	class Spore;
	using P_Spore = std::shared_ptr<Spore>;
	using PW_Spore = std::weak_ptr<Spore>;

	class Path;
	using P_Path = std::shared_ptr<Path>;

	class Pin;
	using P_Pin = std::shared_ptr<Pin>;

	class Path;
	using P_Path = std::shared_ptr<Path>;

	class Session;
	using P_Session = std::shared_ptr<Session>;

	class Context;

	using Pin_Process = std::function< void(Context& cc, P_Data data)>;

}