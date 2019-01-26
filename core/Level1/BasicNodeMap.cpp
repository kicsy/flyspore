#include "BasicNodeMap.h"
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{
		BasicNodeMap::BasicNodeMap(std::weak_ptr< BasicNodeOperator> op) :
			BasicNode(op, any(innerValueType()))
		{
			_mode = NodeMode::AsMap;
		}

		BasicNodeMap::BasicNodeMap() :
			BasicNodeMap(std::shared_ptr< BasicNodeOperator>())
		{

		}

		uint32_t BasicNodeMap::setMode(uint16_t mode)
		{
			_mode = NodeMode::AsMap & mode;
			return mode;
		}

		std::string BasicNodeMap::getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const
		{
			auto collection = std::dynamic_pointer_cast<NodeCollectionVisitor>(_parent.lock());
			std::string pre;
			if (collection)
			{
				pre = collection->getItemPath(shared_from_this());
			}
			std::string nodeKey;
			try
			{
				nodeKey = std::any_cast<std::string>(itemNode->mark());
			}
			catch (const std::bad_any_cast&)
			{
				return std::string();
			}
			return pre + std::string("/") + nodeKey;
		}

		std::shared_ptr<fs::L1::BasicNode> BasicNodeMap::clone() const
		{
			auto _op = opr();
			if (!_op)
			{
				return nullptr;
			}
			auto pnode = _op->createMap();
			pnode->_mode = _mode;
			for (auto &key : keys())
			{
				auto pchild = get(key);
				if (pchild)
				{
					pnode->set(key, pchild->clone());
				}
			}
			return pnode;
		}

		std::vector<std::string> BasicNodeMap::keys() const
		{
			shared_lock_const_node lock(*this);
			auto &map = ref<innerValueType>();
			std::vector<std::string> ks;
			for (auto& x : map)
			{
				ks.push_back(x.first);
			}
			return ks;
		}

		void BasicNodeMap::foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const
		{
			if (p)
			{
				shared_lock_const_node lock(*this);
				auto &map = ref<innerValueType>();
				for (auto& x : map)
				{
					if (!p(x.first, x.second))
					{
						break;
					}
				}
			}
		}

		std::shared_ptr<BasicNode> BasicNodeMap::remove(const std::string &key)
		{
			auto _op = opr();
			if (!_op || !_op->remove(shared_from_this(), key))
			{
				return nullptr;
			}
			std::shared_ptr<BasicNode> node;
			lock();
			auto &map = ref<innerValueType>();
			auto iter = map.find(key);
			if (iter != map.end())
			{
				node = iter->second;
				map.erase(iter);
			}
			unlock();
			if (node)
			{
				_op->onRemoved(shared_from_this(), key, node);
			}
			return std::move(node);
		}

		std::shared_ptr<BasicNode> BasicNodeMap::get(const std::string &key) const
		{
			shared_lock_const_node lock(*this);
			auto &map = ref<innerValueType>();
			auto iter = map.find(key);
			if (iter == map.end() || !iter->second)
			{
				return std::shared_ptr<BasicNode>();
			}
			return iter->second;
		}

		std::shared_ptr<BasicNode> BasicNodeMap::set(const std::string &key, const std::shared_ptr<BasicNode>& node)
		{
			auto _op = opr();
			if (!node || node->parent() || !_op)
			{
				return nullptr;
			}
			auto oldNode = get(key);
			if (oldNode)
			{
				if (!_op->replace(shared_from_this(), key, oldNode, node))
				{
					return nullptr;
				}
				lock();
				auto &map = ref<innerValueType>();
				map[key] = node;
				unlock();
				_op->onReplaced(shared_from_this(), key, node);
			}
			else
			{
				if (!_op->add(shared_from_this(), key, node))
				{
					return nullptr;
				}
				lock();

				auto &map = ref<innerValueType>();
				map[key] = node;
				unlock();
				_op->onAdded(shared_from_this(), key, node);
			}
			return node;
		}

		bool BasicNodeMap::contains(const std::string &key) const
		{
			shared_lock_const_node lock(*this);
			auto &map = ref<innerValueType>();
			return map.find(key) != map.end();
		}
	}
}
