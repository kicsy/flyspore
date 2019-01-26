#include "BasicNodeList.h"
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{

		BasicNodeList::BasicNodeList(std::weak_ptr< BasicNodeOperator> op) : 
			BasicNode(op, any(innerValueType()))
		{
			_mode = NodeMode::AsList;
		}

		BasicNodeList::BasicNodeList() :
			BasicNodeList(std::shared_ptr< BasicNodeOperator>())
		{

		}

		uint32_t BasicNodeList::setMode(uint32_t mode)
		{
			_mode = NodeMode::AsList & mode;
			return mode;

		}

		std::string BasicNodeList::getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const
		{
			auto collection = std::dynamic_pointer_cast<NodeCollectionVisitor>(_parent.lock());
			std::string pre;
			if (collection)
			{
				pre = collection->getItemPath(shared_from_this());
			}
			innerValueType::size_type nodeIndex;
			try
			{
				nodeIndex = std::any_cast<innerValueType::size_type>(itemNode->mark());
			}
			catch (const std::bad_any_cast&)
			{
				return std::string();
			}
			return pre + std::string("[") + std::to_string(nodeIndex) + std::string("]");
		}

		std::shared_ptr<BasicNode> BasicNodeList::clone() const
		{
			auto _op = opr();
			if (!_op)
			{
				return nullptr;
			}
			auto pnode = _op->createList();
			pnode->_mode = _mode;
			for (innerValueType::size_type n = 0; n < length(); n ++)
			{
				auto child = get(n);
				if (child)
				{
					pnode->add(child->clone());
				}
			}
			return pnode;
		}

		BasicNodeList::innerValueType::size_type BasicNodeList::length() const
		{
			shared_lock_const_node lock(*this);
			auto &v = ref<innerValueType>();
			return v.size();
		}

		void BasicNodeList::foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const
		{
			shared_lock_const_node lock(*this);
			auto &v = ref<innerValueType>();
			innerValueType::size_type index = 0;
			for (auto& x : v)
			{
				if (!p(index++, x))
				{
					break;
				}
			}
		}

		std::shared_ptr<BasicNode> BasicNodeList::get(innerValueType::size_type index) const
		{
			shared_lock_const_node lock(*this);
			auto &v = ref<innerValueType>();
			if (index < 0 || index >= v.size())
			{
				return nullptr;
			}
			return v.at(index);
		}

		std::shared_ptr<BasicNode> BasicNodeList::set(innerValueType::size_type index, const std::shared_ptr<BasicNode>& node)
		{
			auto _op = opr();
			if (!node || node->parent() || !_op)
			{
				return nullptr;
			}
			lock();
			auto &v = ref<innerValueType>();
			if (index <= 0 || index > v.size())
			{
				unlock();
				return nullptr;
			}
			unlock();
			if (!_op->replace(shared_from_this(), index, v[index], node))
			{
				return nullptr;
			}
			lock();
			v[index] = node;
			unlock();
			_op->onReplaced(shared_from_this(), index, node);
			return node;
		}

		std::shared_ptr<BasicNode> BasicNodeList::add(const std::shared_ptr<BasicNode>& node, innerValueType::size_type index)
		{
			auto _op = opr();
			if (!node || node->parent() || !_op)
			{
				return nullptr;
			}
			lock();
			auto &v = ref<innerValueType>();
			if (index == -1 && _op->add(shared_from_this(), index, node))
			{
				v.push_back(node);
				unlock();
				_op->onAdded(shared_from_this(), v.size() - 1, node);
				return node;
			}
			else if (index >= 0 && index < v.size() && _op->add(shared_from_this(), index, node))
			{
				v.insert(v.cbegin() + index, node);
				unlock();
				_op->onAdded(shared_from_this(), index, node);
				return node;
			}
			unlock();
			return nullptr;
		}

		std::shared_ptr<BasicNode> BasicNodeList::remove(innerValueType::size_type index)
		{
			auto _op = opr();
			if (!_op || _op->remove(shared_from_this(), index))
			{
				return nullptr;
			}
			lock();
			auto &v = ref<innerValueType>();
			if (index < 0 || index >= v.size())
			{
				unlock();
				return nullptr;
			}
			std::shared_ptr<BasicNode> node = v[index];
			v.erase(v.cbegin() + index);
			unlock();
			if (node)
			{
				_op->onRemoved(shared_from_this(), index, node);
			}
			return node;
		}
	}
}
