#include "BasicNodeMap.h"
#include "BasicNodeOperator.h"
namespace fs
{
	namespace L1
	{
		BasicNodeMap::BasicNodeMap(const std::weak_ptr< BasicNodeOperator>& op) :
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
			bool isok = false;
			std::string nodeKey = itemNode->mark<std::string>(&isok);
			return isok ? (pre + std::string("/") + nodeKey) : std::string("");
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
					pnode->set(pchild->clone(), key);
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

		std::shared_ptr<BasicNode> BasicNodeMap::get(const any &key) const
		{
			bool isok = false;
			auto keyname = getAny<std::string>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}
			shared_lock_const_node lock(*this);
			auto &map = ref<innerValueType>();
			auto iter = map.find(keyname);
			if (iter == map.end() || !iter->second)
			{
				return std::shared_ptr<BasicNode>();
			}
			return iter->second;
		}

		std::shared_ptr<BasicNode> BasicNodeMap::set(const std::shared_ptr<BasicNode>& node, const any &key)
		{
			bool isok = false;
			auto keyname = getAny<std::string>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}

			auto _op = opr();
			if (!node || node->parent() || !_op)
			{
				return nullptr;
			}
			auto oldNode = get(keyname);
			if (oldNode)
			{
				if (!_op->replace(shared_from_this(), key, oldNode, node))
				{
					return nullptr;
				}
				lock();
				auto &map = ref<innerValueType>();
				map[keyname] = node;
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
				map[keyname] = node;
				unlock();
				_op->onAdded(shared_from_this(), key, node);
			}
			return node;
		}

		std::shared_ptr<BasicNode> BasicNodeMap::add(const std::shared_ptr<BasicNode>& node, const any &key /*= any()*/)
		{
			return set(node, key);
		}

		bool BasicNodeMap::contains(const any &key) const
		{
			bool isok = false;
			auto keyname = getAny<std::string>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}

			shared_lock_const_node lock(*this);
			auto &map = ref<innerValueType>();
			return map.find(keyname) != map.end();
		}

		std::shared_ptr<BasicNode> BasicNodeMap::remove(const any &key)
		{
			bool isok = false;
			auto keyname = getAny<std::string>(key, &isok);
			if (!isok)
			{
				return nullptr;
			}

			auto _op = opr();
			if (!_op || !_op->remove(shared_from_this(), key))
			{
				return nullptr;
			}
			std::shared_ptr<BasicNode> node;
			lock();
			auto &map = ref<innerValueType>();
			auto iter = map.find(keyname);
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

	}
}
