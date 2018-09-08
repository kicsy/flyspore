#pragma once
#include <memory>
#include "Object.h"
namespace fs
{
	class Pin;
	class DataPack;
	class Path;
	using P_Path = std::shared_ptr<Path>;
	class Path : public Object<>
	{
	public:
		~Path();
		Path(const Path&) = delete;
		Path& operator=(const Path&) = delete;
		std::shared_ptr<Pin> inPin() const;
		std::shared_ptr<Pin> outPin() const ;
		bool isvalid() const;
	protected:
		std::shared_ptr<Traceable> parent() = delete;
		std::vector<std::shared_ptr<Traceable>> childs() = delete;

		using Object::Object;
		bool move(P_Data data);
	protected:
		std::shared_ptr<Pin> _inPin;
		std::shared_ptr<Pin> _outPin;

		friend class Pin;
	};
}


