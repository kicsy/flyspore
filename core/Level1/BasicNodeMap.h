#pragma once
#include "BasicNode.h"
namespace fs
{
	namespace L1
	{
		class BasicNodeList;
		class BasicNodeMap : public BasicNode, public NodeCollectionVisitor
		{
		public:
			using innerValueType = std::unordered_map<std::string, std::shared_ptr<BasicNode>>;
			friend BasicNodeOperator;

			virtual uint32_t setMode(uint16_t mode);
			virtual std::shared_ptr<BasicNode> clone() const override;

			virtual std::string getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const override;
			void foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const override;
			virtual std::shared_ptr<BasicNode> get(const any &key) const override;
			virtual std::shared_ptr<BasicNode> set(const std::shared_ptr<BasicNode>& value, const any &key) override;
			virtual std::shared_ptr<BasicNode> add(const std::shared_ptr<BasicNode>& node, const any &key = any()) override;
			virtual bool contains(const any &key) const override;
			virtual std::shared_ptr<BasicNode> remove(const any &key) override;
		protected:
			BasicNodeMap();
			explicit BasicNodeMap(const std::weak_ptr< BasicNodeOperator>& op);
			std::vector<std::string> keys() const;
			template<typename vT> vT get() const = delete;
			template<typename vT> void set(const vT&) = delete;
			template<typename vT> bool is_a() const = delete;
			bool has_value() = delete;
		};
	}

}