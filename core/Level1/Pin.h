#pragma once
#include <vector>
#include <shared_mutex>
#include "Data.h"
#include "BasicNodeMap.h"
namespace fs
{
	namespace L1
	{
		enum class Pin_Type : unsigned int
		{
			UNKNOWN = 0,
			IN_PIN,
			OUT_PIN
		};

		class DefaultNest;
		class Pin : public BasicNodeMap
		{
		public:
			virtual ~Pin();
			Pin_Type type() const;
			//std::vector<P_Path> paths() const;
			//P_Spore spore() const;
			//std::string name() const;
			//std::string  reName(const std::string &name);
			virtual bool push(const std::shared_ptr<Data> &data);
			virtual P_DataAdapter adapter() const;
		protected:
			Pin(const std::weak_ptr<DefaultNest>& pNest, Pin_Type type);
			Pin(const Pin&) = delete;
			Pin& operator=(const Pin&) = delete;
			void Pin::task_process(const P_Data &data)
			bool addPath(P_Path path);
			bool removePath(P_Path path);
			virtual void process(Context& ct, const P_Data& pdata) {}
			virtual bool enableProcess() const { return false; }
		protected:
			Pin_Type _type;
			mutable std::shared_mutex _mutex;
			std::vector<P_Path> _outPaths;
			std::vector<P_Path> _inPaths;
			PW_Spore _spore;
			friend class Spore;
			friend class Path;
		};
	}
}


