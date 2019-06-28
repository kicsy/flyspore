#pragma once
#include <vector>
#include <shared_mutex>
#include <atomic>
#include "Data.h"
#include "Context.h"
#include "../Level0/lockfree_vector.hpp"
using namespace fs::L0;
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
			virtual DataAdapter* adapter();
			std::shared_ptr<DefaultNest> nest() const;
			virtual bool push(const std::shared_ptr<Data>& data);
			virtual std::shared_ptr<Path> connect(const std::shared_ptr<Pin>& to, const std::string& name = "");
			virtual std::shared_ptr<Pin> clone(const std::string& newName = std::string("")){return nullptr;};
		protected:
			Pin(const std::weak_ptr<DefaultNest>& pNest, const std::string& name, Pin_Type type);
			Pin(const Pin&) = delete;
			Pin& operator=(const Pin&) = delete;
			void task_process(const std::shared_ptr<Data>&data);
			virtual void process(Context& ct, const std::shared_ptr<Data>&pdata) {}
			virtual bool enableProcess() const { return false; }
		protected:
			Pin_Type _type;
			std::string _name;
			lockfree_vector<std::shared_ptr<Path>> _outPaths;
			lockfree_vector<std::shared_ptr<Path>> _inPaths;
			std::weak_ptr<Spore> _spore;
			std::weak_ptr<DefaultNest> _nest;
			friend class Spore;
			friend class Path;
		};
	}
}


