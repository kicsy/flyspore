#pragma once
#include <memory>
#include <string>
namespace fs
{
	namespace L1
	{
		class DefaultNest;
		class Pin;
		class Data;
		class Spore;
		class Path : public std::enable_shared_from_this<Path>
		{
		public:
			~Path();
			Path(const Path&) = delete;
			Path& operator=(const Path&) = delete;
			std::shared_ptr<Pin> from() const;
			std::shared_ptr<Pin> to() const;
			std::shared_ptr<Spore> spore() const;
			std::string name() const;
			bool isvalid() const;
			bool move(const std::shared_ptr<Data>& data);
			static std::shared_ptr<Path> connect(const std::shared_ptr<Pin> &from, const std::shared_ptr<Pin> &to, const std::string &name = "");
			static std::shared_ptr<Path> connect(std::shared_ptr<Spore> &fromSpore, const std::string &fromPinName,
				std::shared_ptr<Spore> &toSpore, const std::string &toPinName, const std::string &name = "");
			static bool release(const std::shared_ptr<Path> &path);
		protected:
			Path(const std::weak_ptr<Spore>& spore, const std::shared_ptr<Pin>& from, const std::shared_ptr<Pin>& to, const std::string& name);
			bool attach();
			bool detach();
		protected:
			std::string _name;
			std::shared_ptr<Pin> _from;
			std::shared_ptr<Pin> _to;
			std::weak_ptr<Spore> _spore;
			friend class DefaultNest;
			friend class Spore;
		};
	}
}


