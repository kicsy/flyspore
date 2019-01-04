#pragma once
#include "BasicNode.h"
namespace fs
{
	namespace L1
	{

		class Property : public BasicNode, public std::enable_shared_from_this<Property>
		{
		protected:
			virtual PW_BasicNode  weakFromThis()
			{
				return std::static_pointer_cast<BasicNode>(shared_from_this());
			}
		};
		template<class... _Types>
		P_BasicNode make_Property(_Types&&... _Args)
		{
			return std::make_shared<Property>(std::forward<_Types>(_Args)...);
		}

		class PropertyMap : public BasicNodeMap, public std::enable_shared_from_this<PropertyMap>
		{
		protected:
			virtual PW_BasicNode  weakFromThis()
			{
				return std::static_pointer_cast<BasicNode>(shared_from_this());
			}
		};

		template<class... _Types>
		P_BasicNode make_PropertyMap(_Types&&... _Args)
		{
			return  std::dynamic_pointer_cast<BasicNode>(std::make_shared<PropertyMap>(std::forward<_Types>(_Args)...));
		}

		class PropertyList : public BasicNodeList, public std::enable_shared_from_this<PropertyList>
		{
		protected:
			virtual PW_BasicNode  weakFromThis()
			{
				return std::static_pointer_cast<BasicNode>(shared_from_this());
			}
		};

		template<class... _Types>
		P_BasicNode make_PropertyList(_Types&&... _Args)
		{
			return  std::dynamic_pointer_cast<BasicNode>(std::make_shared<PropertyList>(std::forward<_Types>(_Args)...));
		}
	}

}