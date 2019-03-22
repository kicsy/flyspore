#include "DefaultNest.h"
#include "Spore.h"
#include "Path.h"
#include "WarpPin.h"
namespace fs
{
	namespace L1
	{

		DefaultNest::DefaultNest()
		{

		}


		DefaultNest::~DefaultNest()
		{
		}

		std::shared_ptr<Spore> DefaultNest::createSpore(const std::string& name)
		{
			return std::shared_ptr<Spore>(new Spore(weak_from_this(), name));
		}

		std::shared_ptr<Pin> DefaultNest::createPin(const std::string& name, Pin_Type type, DefaultWarpPin::ProcessType proess)
		{
			return std::shared_ptr<Pin>(new DefaultWarpPin(name, type, proess));
		}

		std::shared_ptr<Pin> DefaultNest::createPin(const std::string& name, DefaultWarpPin::ProcessType proess)
		{
			return std::shared_ptr<Pin>(new DefaultWarpPin(name, Pin_Type::IN_PIN, proess));
		}

		void DefaultNest::onAddSpore(std::shared_ptr<Spore> parent, std::shared_ptr<Spore> child)
		{

		}

		void DefaultNest::onRemoveSpore(std::shared_ptr<Spore> parent, std::shared_ptr<Spore> child)
		{
		}

		void DefaultNest::onAddPin(std::shared_ptr<Spore> parent, std::shared_ptr<Pin> child)
		{

		}

		void DefaultNest::onRemovePin(std::shared_ptr<Spore> parent, std::shared_ptr<Pin> child)
		{
		}

		void DefaultNest::onAddPath(std::shared_ptr<Spore> spore, std::shared_ptr<Path> path)
		{
		}

		void DefaultNest::onRemovePath(std::shared_ptr<Spore> spore, std::shared_ptr<Path> path)
		{
		}


		void DefaultNest::lock() const
		{
			_sync.lock();
		}

		void DefaultNest::unlock() const
		{
			_sync.unlock();
		}

		void DefaultNest::lock_shared() const
		{
			_sync.lock_shared();
		}

		void DefaultNest::unlock_shared() const
		{
			_sync.unlock_shared();
		}

		bool DefaultNest::try_lock() const
		{
			return _sync.try_lock();
		}

		bool DefaultNest::try_lock_shared() const
		{
			return _sync.try_lock_shared();
		}
	}
}