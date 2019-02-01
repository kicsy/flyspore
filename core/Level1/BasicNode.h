#pragma once
#include <string>
#include  <utility>
#include <memory>
#include <shared_mutex>
#include <any>
#include <unordered_map>
#include <vector>
namespace fs
{
	namespace L1
	{
		enum NodeMode : uint32_t {
			AsNode=			0x0001 << 0,
			AsList =			0x0001 << 1,
			AsMap =			0x0001 << 2
		};
		using any = std::any;
		class BasicNode;
		class BasicNodeOperator;

		class NodeCollectionVisitor
		{
		public:
			virtual std::string getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const = 0;
			virtual void foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const = 0;
			virtual std::shared_ptr<BasicNode> get(const any &key) const = 0;
			virtual std::shared_ptr<BasicNode> set(const std::shared_ptr<BasicNode>& value, const any &key) = 0;
			virtual std::shared_ptr<BasicNode> add(const std::shared_ptr<BasicNode>& node, const any &key = any()) = 0;
			virtual bool contains(const any &key) const = 0;
			virtual std::shared_ptr<BasicNode> remove(const any &key) =0;
		};
		

		class BasicNode : public std::enable_shared_from_this<BasicNode>
		{
		public:
			using shared_lock_const_node = std::shared_lock<const BasicNode>;
			using unique_lock_const_node = std::unique_lock<const BasicNode>;
			friend shared_lock_const_node;
			friend unique_lock_const_node;
			friend BasicNodeOperator;
			~BasicNode();
			BasicNode& operator=(const BasicNode& other);
			std::shared_ptr<BasicNode> parent() const;
			virtual uint32_t mode() const;
			virtual uint32_t setMode(uint32_t mode);
			virtual std::shared_ptr<BasicNode> clone() const;
			bool has_value() const;
			void swap(BasicNode &&other);
			std::string path() const;
			template<typename vT> 
			vT value() const;
			template<typename vT>
			void setValue(const vT& value);
			template<typename vT>
			bool is_a() const;
			template<typename vT>
			vT mark(bool *isok = nullptr) const;
			std::shared_ptr< BasicNodeOperator> opr() const;
			template<typename vT>
			vT& ref();
			template<typename vT>
			const vT& ref() const;

			template<typename vT>
			static vT getAny(const any& value, bool *isok = nullptr);
		protected:
			BasicNode(const std::weak_ptr<BasicNodeOperator>& op, const any  &value = any());
			BasicNode(const BasicNode& other);
			BasicNode(BasicNode&& other);
			void lock() const;
			void unlock() const;
			void lock_shared() const;
			void unlock_shared() const;
			bool try_lock() const;
			bool try_lock_shared() const;

		protected:
			std::weak_ptr<BasicNode> _parent;
			any _value;
			any _mark;
			std::weak_ptr<BasicNodeOperator> _operator;
			uint32_t _mode{0};
		};

		template<typename vT>
		vT BasicNode::value() const
		{
			shared_lock_const_node lock(*this);
			return ref<vT>();
		}

		template<typename vT>
		void BasicNode::setValue(const vT& value)
		{
			auto _op = opr();
			if (!_op || !_op->modify(shared_from_this(), any(value)))
			{
				return;
			}
			lock();
			_value = value;
			unlock();
			_op->onModified(shared_from_this());
		}

		template<typename vT>
		bool BasicNode::is_a() const
		{
			shared_lock_const_node lock(*this);
			return _value.type() == typeid(vT);
		}

		template<typename vT>
		vT BasicNode::mark(bool *isok) const
		{
			bool _ok = false;
			shared_lock_const_node lock(*this);
			return getAny<vT>(_mark, isok);
		}

		template<typename vT>
		vT& BasicNode::ref()
		{
			return std::any_cast<vT&>(_value);
		}

		template<typename vT>
		const vT& BasicNode::ref() const
		{
			return std::any_cast<const vT&>(_value);
		}

		template<typename vT>
		vT BasicNode::getAny(const any& value, bool *isok /*= nullptr*/)
		{
			bool _ok = false;
			
			try
			{
				vT _v = std::any_cast<vT>(value);
				if(isok) *isok = true;
				return _v;
			}
			catch (const std::bad_any_cast&)
			{
				if (isok) *isok = false;
				return vT();
			}
			return vT();
		}
	}

}