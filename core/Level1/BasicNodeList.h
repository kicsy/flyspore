#pragma once
#include "BasicNode.h"
namespace fs
{
	namespace L1
	{
		class BasicNodeList : public BasicNode, public NodeCollectionVisitor
		{
		public:
			using innerValueType = std::vector<std::shared_ptr<BasicNode>>;
			friend BasicNodeOperator;

			virtual uint32_t setMode(uint32_t mode) override;
			virtual std::string getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const override;
			virtual std::shared_ptr<BasicNode> clone() const override;

			innerValueType::size_type length() const;
			void foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const;
			std::shared_ptr<BasicNode> get(innerValueType::size_type index) const;
			std::shared_ptr<BasicNode> set(innerValueType::size_type index, const std::shared_ptr<BasicNode>& value);
			std::shared_ptr<BasicNode> add(const std::shared_ptr<BasicNode>& node, innerValueType::size_type index = -1);
			std::shared_ptr<BasicNode> remove(innerValueType::size_type index);
		protected:
			BasicNodeList();
			explicit BasicNodeList(std::weak_ptr< BasicNodeOperator> op);
			template<typename vT> vT get() const = delete;
			template<typename vT> void set(const vT&) = delete;
			template<typename vT> bool is_a() const = delete;
			bool has_value() = delete;

		};
	}

}