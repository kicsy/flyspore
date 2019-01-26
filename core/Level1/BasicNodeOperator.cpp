#include "BasicNodeOperator.h"
#include "BasicNode.h"
#include "BasicNodeMap.h"
#include "BasicNodeList.h"
#include <iostream>
namespace fs
{
	namespace L1
	{

		BasicNodeOperator::BasicNodeOperator()
		{

		}

		BasicNodeOperator::~BasicNodeOperator()
		{

		}

		std::shared_ptr<BasicNodeMap> BasicNodeOperator::root()
		{
			if (!_root)
			{
				_root = createMap();
			}
			return _root;
		}

		std::shared_ptr<BasicNode> BasicNodeOperator::create(const std::any& value)
		{
			auto pnode = std::shared_ptr<BasicNode>(new BasicNode(weak_from_this(), value));
			onCreated(pnode);
			return pnode;
		}

		std::shared_ptr<BasicNodeMap> BasicNodeOperator::createMap()
		{
			auto pnode = std::shared_ptr<BasicNodeMap>(new BasicNodeMap(weak_from_this()));
			onCreated(pnode);
			return pnode;

		}

		std::shared_ptr<BasicNodeList> BasicNodeOperator::createList()
		{
			auto pnode = std::shared_ptr<BasicNodeList>(new BasicNodeList(weak_from_this()));
			onCreated(pnode);
			return pnode;
		}

		void BasicNodeOperator::onCreated(const std::shared_ptr<BasicNode>& pnode)
		{
			_nodecount++;
			std::cout << "create node: " << _nodecount << std::endl;
		}

		void BasicNodeOperator::onReleased(BasicNode* pnode)
		{
			_nodecount--;
			std::cout << "release node: " << _nodecount << std::endl;
		}

		bool BasicNodeOperator::modify(const std::shared_ptr<BasicNode>& pnode, const std::any& value)
		{
			return true;
		}

		void BasicNodeOperator::onModified(const std::shared_ptr<BasicNode> & pnode)
		{

		}

		bool BasicNodeOperator::replace(const std::shared_ptr<BasicNode> & parent, const std::any & key, const std::shared_ptr<BasicNode> & from, const std::shared_ptr<BasicNode> & to)
		{
			return true;
		}

		void BasicNodeOperator::onReplaced(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& pnode)
		{
			if (parent && pnode)
			{
				pnode->_parent = parent->weak_from_this();
				pnode->_mark = key;
				std::cout << "replace node: " << pnode->path() << std::endl;
			}
		}

		bool BasicNodeOperator::add(const std::shared_ptr<BasicNode> & parent, const std::any & key, const std::shared_ptr<BasicNode> & pnode)
		{
			return true;
		}

		void BasicNodeOperator::onAdded(const std::shared_ptr<BasicNode> & parent, const std::any & key, const std::shared_ptr<BasicNode> & pnode)
		{
			if (parent && pnode)
			{
				pnode->_parent = parent->weak_from_this();
				pnode->_mark = key;
				std::cout << "add node: " << pnode->path() << std::endl;
			}
		}

		bool BasicNodeOperator::remove(const std::shared_ptr<BasicNode>& parent, const std::any& key)
		{
			return true;
		}

		void BasicNodeOperator::onRemoved(const std::shared_ptr<BasicNode> & parent, const std::any & key, const std::shared_ptr<BasicNode> & pnode)
		{
			if (pnode)
			{
				std::cout << "remove node: " << pnode->path() << std::endl;
				pnode->_parent = std::shared_ptr<BasicNode>();
			}
		}

		void BasicNodeOperator::lock(const std::shared_ptr<const BasicNode>&) const
		{
			_sync.lock();
		}

		void BasicNodeOperator::unlock(const std::shared_ptr<const BasicNode> &) const
		{
			_sync.unlock();
		}

		void BasicNodeOperator::lock_shared(const std::shared_ptr<const BasicNode> &) const
		{
			_sync.lock_shared();
		}

		void BasicNodeOperator::unlock_shared(const std::shared_ptr<const BasicNode>&) const
		{
			_sync.unlock_shared();
		}

		bool BasicNodeOperator::try_lock(const std::shared_ptr<const BasicNode> &) const
		{
			return _sync.try_lock();
		}

		bool BasicNodeOperator::try_lock_shared(const std::shared_ptr<const BasicNode> &) const
		{
			return _sync.try_lock_shared();
		}
	}
}