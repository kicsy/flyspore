#pragma once
#include <cassert>
#include "Statement.h"
#include "AnyValues.h"
namespace fs
{
	template< typename ProcessType = void(Context&, AnyValues&)>
	class Schema
	{
	public:
		Schema() {}
		virtual ~Schema() {}
		using TransformFunc = void(P_Data& pdata, void* pfunc);
		using ProcessFunc = void(Context& cc, AnyValues& data);
		virtual void process(P_Data& pdata, void* pfunc)
		{

		}
		virtual bool isMatch(Schema* other);
		virtual P_Data createData(const AnyValues &values);
	protected:
		void transform(P_Data& pdata, void* pfunc)
		{
			assert(pfunc);
			processType pf = (processType)(pfunc);

		}
	protected:
		friend Spore;
	};

}

