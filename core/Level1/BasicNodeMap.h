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
			virtual std::string getItemPath(const std::shared_ptr<const BasicNode>& itemNode) const override;
			virtual std::shared_ptr<BasicNode> clone() const override;

			std::vector<std::string> keys() const;
			void foreach(std::function<bool(const any& mark, const std::shared_ptr<BasicNode>& child)> p) const;
			std::shared_ptr<BasicNode> get(const std::string &key) const;
			std::shared_ptr<BasicNode> set(const std::string &key, const std::shared_ptr<BasicNode>& value);
			bool contains(const std::string &key) const;
			std::shared_ptr<BasicNode> remove(const std::string &key);
		protected:
			BasicNodeMap();
			explicit BasicNodeMap(std::weak_ptr< BasicNodeOperator> op);

			template<typename vT> vT get() const = delete;
			template<typename vT> void set(const vT&) = delete;
			template<typename vT> bool is_a() const = delete;
			bool has_value() = delete;
		};
	}

}