#pragma once
#include <memory>
#include <shared_mutex>
#include "WarpPin.h"
namespace fs
{
	namespace L1
	{
		class Spore;
		class Pin;
		class Path;
		class DefaultNest : public std::enable_shared_from_this<DefaultNest>
		{
		public:
			friend class NestLock;
			DefaultNest();
			~DefaultNest();
			std::shared_ptr<Spore> createSpore(const std::string& name);
			std::shared_ptr<Pin> createPin(const std::string& name, Pin_Type type = Pin_Type::OUT_PIN, WarpPin<>::InnerProcessType proess = nullptr);
			std::shared_ptr<Pin> createPin(const std::string& name, WarpPin<>::InnerProcessType proess);
			template<typename DataType, typename SchemaType = LightSchema<typename DataType>>
			std::shared_ptr<Pin> createPin(const std::string& name, Pin_Type type = Pin_Type::OUT_PIN, typename WarpPin<typename DataType, typename SchemaType>::InnerProcessType proess = nullptr);
			template<typename DataType, typename SchemaType = LightSchema<typename DataType>>
			std::shared_ptr<Pin> createPin(const std::string& name, typename WarpPin<typename DataType, typename SchemaType>::InnerProcessType proess = nullptr);
			virtual void lock() const;
			virtual void unlock() const;
			virtual void lock_shared() const;
			virtual void unlock_shared() const;
			virtual bool try_lock() const;
			virtual bool try_lock_shared() const;

		protected:
			virtual void onAddSpore(std::shared_ptr<Spore> parent, std::shared_ptr<Spore> child);
			virtual void onRemoveSpore(std::shared_ptr<Spore> parent, std::shared_ptr<Spore> child);
			virtual void onAddPin(std::shared_ptr<Spore> parent, std::shared_ptr<Pin> child);
			virtual void onRemovePin(std::shared_ptr<Spore> parent, std::shared_ptr<Pin> child);
			virtual void onAddPath(std::shared_ptr<Spore> spore, std::shared_ptr<Path> path);
			virtual void onRemovePath(std::shared_ptr<Spore> spore, std::shared_ptr<Path> path);


		protected:
			mutable std::shared_mutex _sync;
			friend class Spore;
		};

		template<typename DataType, typename SchemaType>
		std::shared_ptr<Pin>
			DefaultNest::createPin(const std::string& name, Pin_Type type /*= Pin_Type::OUT_PIN*/, typename WarpPin<typename DataType, typename SchemaType>::InnerProcessType proess /*= nullptr*/)
		{
			return std::shared_ptr<Pin>(new WarpPin<DataType, SchemaType>(name, type, proess));
		}

		template<typename DataType, typename SchemaType>
		std::shared_ptr<Pin>
			DefaultNest::createPin(const std::string& name, typename WarpPin<typename DataType, typename SchemaType>::InnerProcessType proess /*= nullptr*/)
		{
			return std::shared_ptr<Pin>(new WarpPin<DataType, SchemaType>(name, Pin_Type::IN_PIN, proess));
		}

		class NestSharedLock : public std::shared_lock<DefaultNest>
		{
		public:
			NestSharedLock(const std::weak_ptr<DefaultNest>& pNest):
				std::shared_lock<DefaultNest>(*pNest.lock()){}
			NestSharedLock(const std::shared_ptr<DefaultNest>& pNest) :
				std::shared_lock<DefaultNest>(*pNest){}
		};
		class NestUniqueLock : public std::unique_lock<DefaultNest>
		{
		public:
			NestUniqueLock(const std::weak_ptr<DefaultNest>& pNest) :
				std::unique_lock<DefaultNest>(*pNest.lock()) {}
			NestUniqueLock(const std::shared_ptr<DefaultNest>& pNest) :
				std::unique_lock<DefaultNest>(*pNest) {}
		};
	}
}
