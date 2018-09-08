#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <atomic>
#include <any>
namespace fs
{

	using IdType = unsigned long long int;

	using std::any_cast;

	class AnyValues;
	using P_AnyValues = std::shared_ptr<AnyValues>;

	class AnyValues : public std::unordered_map<std::string, std::any>
	{
	public:
		AnyValues() = default;
		~AnyValues() {}
		AnyValues(const AnyValues&) = default;

		template<typename vT>
		vT& ref(const std::string &strKey)
		{
			auto iter = _many.find(strKey);
			if (iter != _many.end())
			{
				try
				{
					return std::any_cast<vT&>(_many[strKey]);
				}
				catch (const std::bad_any_cast &){}
			}
			_many[strKey] = vT();
			return std::any_cast<vT&>(_many[strKey]);
		}

		template<typename vT>
		bool is_a(const std::string &strKey)
		{
			if (_many.count(strKey) <= 0)
			{
				return false;
			}
			return _many[strKey].type() == typeid(vT);
		}
	protected:
		std::unordered_map<std::string, std::any> _many;
	};

	class Session;
	class DataPack;
	using P_Session = std::shared_ptr<Session>;
	using P_Data = std::shared_ptr<DataPack>;
	class DataPack : public AnyValues
	{
		public:
		~DataPack() {}
		DataPack(const DataPack&) = delete;
		DataPack& operator=(const DataPack&) = delete;
		P_Session getSession() const;
	protected:
		DataPack(P_Session pss, const AnyValues &any = AnyValues());
	protected:
		P_Session _session;
		friend class Session;
	};

	class Traceable 
	{
	public:
		Traceable();
		virtual ~Traceable();
		static long long int total();
		static IdType newID(IdType preID = 0);
		static std::atomic<long long int> _total;
		static std::atomic<IdType> __baseID;
	};

	template< typename TObject = Traceable, typename TParent = Traceable>
	class Object : public Traceable , public std::enable_shared_from_this<TObject>
	{
	public:
		Object(const std::string  &name, IdType id = 0);
		virtual ~Object();
		AnyValues& propertis();
		std::string name();
		IdType id();
		std::shared_ptr<TParent> parent();
		std::vector<std::shared_ptr<TObject>> childs();
		std::shared_ptr<TObject> getptr();
	protected:
		std::weak_ptr<TParent>_parent;
		std::vector<std::shared_ptr<TObject>> _childs;
		AnyValues _properties;
		IdType _id;
		std::string _name;
	};


	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	fs::Object<TObject, TParent>::Object(const std::string  &name, fs::IdType id /*= 0*/) :
		Traceable()
		, _name(name)
		, _id(id)
	{
		if (_id == 0)
		{
			_id = newID(id);
		}
		if (_name.length() == 0)
		{
			_name = std::string("object_") + std::to_string(_id);
		}
	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	fs::Object<TObject, TParent>::~Object()
	{

	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	fs::AnyValues& fs::Object<TObject, TParent>::propertis()
	{
		return _properties;
	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	std::string fs::Object<TObject, TParent>::name()
	{
		return _name;
	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	IdType fs::Object<TObject, TParent>::id()
	{
		return _id;
	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	std::shared_ptr<TParent> fs::Object<TObject, TParent>::parent()
	{
		return std::move(_parent.lock());
	}

	template< typename TObject /*= Traceable*/, typename TParent /*= Traceable*/>
	std::vector<std::shared_ptr<TObject>> fs::Object<TObject, TParent>::childs()
	{
		std::vector<std::shared_ptr<TObject>> cs;
		for (auto child : _childs)
		{
			cs.push_back(std::make_shared<TObject> && (child));
		}
		return std::move(cs);
	}

	template<typename TObject, typename TParent>
	inline std::shared_ptr<TObject> Object<TObject, TParent>::getptr()
	{
		return std::enable_shared_from_this<TObject>::shared_from_this();
	}


}

