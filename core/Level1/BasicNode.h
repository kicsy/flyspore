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
			bool has_value() const;
			void swap(BasicNode &&other);
			virtual std::shared_ptr<BasicNode> clone() const;
			std::string path() const;
			template<typename vT> 
			vT value() const;
			template<typename vT>
			void setValue(const vT& value);
			template<typename vT>
			bool is_a() const;
			any mark() const;
			std::shared_ptr< BasicNodeOperator> opr() const;
			template<typename vT>
			vT& ref();
			template<typename vT>
			const vT& ref() const;

		protected:
			BasicNode(std::weak_ptr<BasicNodeOperator> op, const any  &value = any());
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
		vT& BasicNode::ref()
		{
			return std::any_cast<vT&>(_value);
		}

		template<typename vT>
		const vT& BasicNode::ref() const
		{
			return std::any_cast<const vT&>(_value);
		}
	}

}