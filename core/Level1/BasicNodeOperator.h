#pragma once
#include <string>
#include <any>
#include <shared_mutex>
#include "BasicNode.h"
#include "BasicNodeList.h"
#include "BasicNodeMap.h"
namespace fs
{
	namespace L1
	{
		class BasicNodeOperator : public std::enable_shared_from_this<BasicNodeOperator>
		{
		public:
			BasicNodeOperator();
			~BasicNodeOperator();
			virtual std::shared_ptr<BasicNodeMap> root();
			virtual std::shared_ptr<BasicNode> create(const std::any& value);
			virtual std::shared_ptr<BasicNode> create(const char* pvalue);
			virtual std::shared_ptr<BasicNodeMap> createMap();
			virtual std::shared_ptr<BasicNodeList> createList();
			virtual bool modify(const std::shared_ptr<BasicNode>& pnode, const std::any& value);
			virtual void onModified(const std::shared_ptr<BasicNode>& pnode);
			virtual bool replace(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& from, const std::shared_ptr<BasicNode>& to);
			virtual void onReplaced(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& pnode);
			virtual bool add(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& pnode);
			virtual void onAdded(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& pnode);
			virtual bool remove(const std::shared_ptr<BasicNode>& parent, const std::any& key);
			virtual void onRemoved(const std::shared_ptr<BasicNode>& parent, const std::any& key, const std::shared_ptr<BasicNode>& pnode);
			virtual void lock(const std::shared_ptr<const BasicNode>& pnode) const;
			virtual void unlock(const std::shared_ptr<const BasicNode>& pnode) const;
			virtual void lock_shared(const std::shared_ptr<const BasicNode>& pnode) const;
			virtual void unlock_shared(const std::shared_ptr<const BasicNode>& pnode) const;
			virtual bool try_lock(const std::shared_ptr<const BasicNode>& pnode) const;
			virtual bool try_lock_shared(const std::shared_ptr<const BasicNode>& pnode) const;

			int getNodeCount() { return _nodecount; }
		protected:
			virtual void onCreated(const std::shared_ptr<BasicNode>& pnode);
			virtual void onReleased(BasicNode* pnode);

		protected:
			mutable std::shared_mutex _sync;
			std::shared_ptr<BasicNodeMap> _root;
			friend class BasicNode;

			int _nodecount{ 0 };
		};
	}

}