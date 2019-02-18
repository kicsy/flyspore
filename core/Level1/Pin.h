#pragma once
#include <vector>
#include <shared_mutex>
#include <atomic>
#include "Data.h"
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
		class Spore;
		class Data;
		class Path;
		class DataAdapter;
		class Pin : public std::enable_shared_from_this<Pin>
		{
		public:
			virtual ~Pin();
			Pin_Type type() const;
			std::shared_ptr<Spore> spore() const;
			std::string name() const;
			virtual std::shared_ptr<DataAdapter> adapter();
			virtual bool push(const std::shared_ptr<Data>& data);
			virtual std::shared_ptr<Path> connect(const std::shared_ptr<Pin>& to, const std::string& name = "");
		protected:
			Pin(const std::string& name, Pin_Type type);
			Pin(const Pin&) = delete;
			Pin& operator=(const Pin&) = delete;
			void Pin::task_process(const std::shared_ptr<Data>&data);
			virtual void process(Context& ct, const std::shared_ptr<Data>&pdata) {}
			virtual bool enableProcess() const { return false; }
		protected:
			Pin_Type _type;
			std::string _name;
			std::vector<std::shared_ptr<Path>> _outPaths;
			std::vector<std::shared_ptr<Path>> _inPaths;
			std::weak_ptr<Spore> _spore;
			friend class Spore;
			friend class Path;
		};
	}
}


