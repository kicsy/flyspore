#include "BasicNode.h"
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{
		BasicNode::BasicNode(std::weak_ptr<BasicNodeOperator> op, const any &value /*= any()*/):
			_operator(op)
			, _value(value)
			, _mode(NodeMode::AsNode)
		{

		}

		BasicNode::BasicNode(const BasicNode& other)
		{
			_value = other._value;
			_operator = other._operator;
		}

		BasicNode::BasicNode(BasicNode&& other)
		{
			swap(std::forward<BasicNode>(other));
		}

		BasicNode::~BasicNode()
		{
			auto _op = opr();
			if (_op)
			{
				_op->onReleased(this);
			}
		}

		std::shared_ptr<BasicNode> BasicNode::parent() const
		{
			return _parent.lock();
		}

		uint32_t BasicNode::mode() const
		{
			return _mode;
		}

		uint32_t BasicNode::setMode(uint32_t mode)
		{
			_mode = mode & NodeMode::AsNode;
			return _mode;
		}

		BasicNode& BasicNode::operator=(const BasicNode& other)
		{
			_value = other._value;
			_mode = other._mode;
			_operator = other._operator;
			return *this;
		}

		bool BasicNode::has_value() const
		{
			return _value.has_value();
		}

		void BasicNode::swap(BasicNode &&other)
		{
			std::swap(_value, other._value);
			std::swap(_parent, other._parent);
			std::swap(_mark, other._mark);
			std::swap(_mode, other._mode);
			std::swap(_operator, other._operator);
		}

		std::shared_ptr<BasicNode> BasicNode::clone() const
		{
			auto _op = opr();
			if (!_op)
			{
				return nullptr;
			}
			auto pnode = _op->create(_value);
			pnode->_mode = _mode;
			return pnode;
		}

		std::string BasicNode::path() const
		{
			std::shared_ptr<NodeCollectionVisitor> collection = std::dynamic_pointer_cast<NodeCollectionVisitor>(_parent.lock());
			if (!collection)
			{
				return std::string();
			}
			return collection->getItemPath(shared_from_this());
		}

		any BasicNode::mark() const
		{
			return _mark;
		}

		std::shared_ptr< BasicNodeOperator> BasicNode::opr() const
		{
			return _operator.lock();
		}

		void BasicNode::lock() const
		{
			if (opr())
			{
				opr()->lock(shared_from_this());
			}
		}

		void BasicNode::unlock() const
		{
			if (opr())
			{
				opr()->unlock(shared_from_this());
			}
		}

		void BasicNode::lock_shared() const
		{
			if (opr())
			{
				opr()->lock_shared(shared_from_this());
			}
		}

		void BasicNode::unlock_shared() const
		{
			if (opr())
			{
				opr()->unlock_shared(shared_from_this());
			}
		}

		bool BasicNode::try_lock() const
		{
			if (opr())
			{
				return opr()->try_lock(shared_from_this());
			}
			return false;
		}

		bool BasicNode::try_lock_shared() const
		{
			if (opr())
			{
				return opr()->try_lock_shared(shared_from_this());
			}
			return false;
		}
	}
}