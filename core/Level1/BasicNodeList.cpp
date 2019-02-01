#include "BasicNodeList.h"
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{

		BasicNodeList::BasicNodeList(const std::weak_ptr< BasicNodeOperator>& op) : 
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

		std::shared_ptr<BasicNode> BasicNodeList::clone() const
		{
			auto _op = opr();
			if (!_op)
			{
				return nullptr;
			}
			auto pnode = _op->createList();
			pnode->_mode = _mode;
			auto &v = ref<innerValueType>();
			for (innerValueType::size_type n = 0; n < v.size(); n ++)
			{
				auto child = get(n);
				if (child)
				{
					pnode->add(child->clone());
				}
			}
			return pnode;
		}

		std::string BasicNodeList::getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const
		{
			auto collection = std::dynamic_pointer_cast<NodeCollectionVisitor>(_parent.lock());
			std::string pre;
			if (collection)
			{
				pre = collection->getItemPath(shared_from_this());
			}
			bool isok = false;
			innerValueType::size_type nodeIndex = itemNode->mark<innerValueType::size_type>(&isok);
			return isok ? (pre + std::string("[") + std::to_string(nodeIndex) + std::string("]")) : std::string("");
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

		std::shared_ptr<BasicNode> BasicNodeList::get(const any &key) const
		{
			bool isok = false;
			auto index = getAny<innerValueType::size_type>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}
			shared_lock_const_node lock(*this);
			auto &v = ref<innerValueType>();
			if (index < 0 || index >= v.size())
			{
				return nullptr;
			}
			return v.at(index);
		}

		std::shared_ptr<fs::L1::BasicNode> BasicNodeList::set(const std::shared_ptr<BasicNode>& node, const any &key)
		{
			bool isok = false;
			auto index = getAny<innerValueType::size_type>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}

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

		std::shared_ptr<fs::L1::BasicNode> BasicNodeList::add(const std::shared_ptr<BasicNode>& node, const any &key /*= any()*/)
		{
			bool isok = false;
			auto index = getAny<innerValueType::size_type>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}

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

		bool BasicNodeList::contains(const any &key) const
		{
			bool isok = false;
			auto index = getAny<innerValueType::size_type>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}
			lock();
			auto &v = ref<innerValueType>();
			isok = index >= 0 && index < v.size();
			unlock();
			return isok;
		}

		std::shared_ptr<fs::L1::BasicNode> BasicNodeList::remove(const any &key)
		{
			bool isok = false;
			auto index = getAny<innerValueType::size_type>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}
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
