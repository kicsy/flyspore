#pragma once
#include <vector>
#include "Statement.h"
#include <shared_mutex>
namespace fs
{
	namespace L1
	{
		class Pin : public std::enable_shared_from_this<Pin>
		{
		public:
			virtual ~Pin();
			Pin_Type type() const;
			std::vector<P_Path> paths() const;
			P_Spore spore() const;
			std::string name() const;
			std::string  reName(const std::string &name);
			virtual void push(const P_Data &data);
			virtual P_DataAdapter adapter() const;
		protected:
			Pin(PW_Spore spore, std::string name, Pin_Type type);
			Pin(const Pin&) = delete;
			Pin& operator=(const Pin&) = delete;
			bool addPath(P_Path path);
			bool removePath(P_Path path);
			virtual void process(Context& ct, const P_Data& pdata) {}
			virtual bool enableProcess() const { return false; }
		protected:
			Pin_Type _type;
			std::string _name;
			mutable std::shared_mutex _mutex;
			std::vector<P_Path> _outPaths;
			std::vector<P_Path> _inPaths;
			PW_Spore _spore;
			friend class Spore;
			friend class Path;
		};
	}
}


