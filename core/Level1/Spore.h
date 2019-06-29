/*!
 * \file Spore.h
 * \date 2016/08/19 12:38
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <map>
#include <unordered_map>
#include "WarpPin.h"

namespace fs
{
	namespace L1
	{
		class DefaultNest;
		class Pin;
		class Path;
		class Spore : public std::enable_shared_from_this<Spore>
		{
		public:
			~Spore();

			std::shared_ptr<Spore> addSpore(const std::shared_ptr<Spore>& spore);
			std::shared_ptr<Spore> remvoeSpore(const std::shared_ptr<Spore>& spore);
			std::shared_ptr<Spore> getSpore(const std::string& name);

			std::shared_ptr<Pin> addPin(const std::shared_ptr<Pin>& pin);
			std::shared_ptr<Pin> removePin(const std::shared_ptr<Pin>& pin);
			std::shared_ptr<Pin> getPin(const std::string& name);

			std::string name() const;
			std::shared_ptr<Spore> parent() const;
			std::shared_ptr<DefaultNest> nest() const;

			std::vector < std::shared_ptr<Pin>> getPins();
			std::vector < std::shared_ptr<Spore>> getChilds();
			std::vector< std::shared_ptr<Path>> getPaths();
			AnyValues& propertys();
			virtual std::shared_ptr<Spore> clone(const std::string& newName = std::string(""));
		protected:
			Spore(const std::weak_ptr<DefaultNest>& pNest, const std::string& name);
			Spore(const Spore&) = delete;
		protected:
			std::vector < std::shared_ptr<Pin>>& _pins();
			std::vector < std::shared_ptr<Spore>>& _childs();
			std::vector < std::shared_ptr<Path>>& _paths();
			AnyValues& _propertys();
			const std::shared_ptr<Path> addPath(const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string &name = "");
			bool removePath(const std::shared_ptr<Path>& path);
			inline std::shared_ptr<Spore> _clone(const std::string& newName, std::map<std::shared_ptr<Pin>, std::shared_ptr<Pin>> &pinmap);
		protected:
			friend class DefaultNest;
			friend class Session;
			friend class Path;
			std::string _name;
			std::weak_ptr<Spore> _parent;
			std::weak_ptr<DefaultNest> _nest;
			std::vector < std::shared_ptr<Pin>> *_ppins;
			std::vector < std::shared_ptr<Spore>> *_pchilds;
			std::vector< std::shared_ptr<Path>> *_ppaths;
			AnyValues *_pvalues;
		};
	}
}


