/* === S Y N F I G ========================================================= */
/*!	\file valuenode_pow.cpp
**	\brief Implementation of the "Power" valuenode conversion.
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007, 2008 Chris Moore
**	Copyright (c) 2009 Nikita Kitaev
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "valuenode_pow.h"
#include "valuenode_const.h"
#include "general.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

ValueNode_Pow::ValueNode_Pow(const ValueBase &x):
	LinkableValueNode(x.get_type())
{
	Real value(x.get(Real()));
	Real infinity(999999.0);
	Real epsilon(0.000001);

	set_link("base",     ValueNode_Const::create(Real(value)));
	set_link("power",    ValueNode_Const::create(Real(1)));
	set_link("epsilon",  ValueNode_Const::create(Real(epsilon)));
	set_link("infinite", ValueNode_Const::create(Real(infinity)));
}

ValueNode_Pow*
ValueNode_Pow::create(const ValueBase &x)
{
	return new ValueNode_Pow(x);
}

LinkableValueNode*
ValueNode_Pow::create_new()const
{
	return new ValueNode_Pow(get_type());
}

ValueNode_Pow::~ValueNode_Pow()
{
	unlink_all();
}

bool
ValueNode_Pow::set_link_vfunc(int i,ValueNode::Handle value)
{
	assert(i>=0 && i<link_count());

	switch(i)
	{
	case 0: CHECK_TYPE_AND_SET_VALUE(base_,     ValueBase::TYPE_REAL);
	case 1: CHECK_TYPE_AND_SET_VALUE(power_,    ValueBase::TYPE_REAL);
	case 2: CHECK_TYPE_AND_SET_VALUE(epsilon_,  ValueBase::TYPE_REAL);
	case 3: CHECK_TYPE_AND_SET_VALUE(infinite_, ValueBase::TYPE_REAL);
	}
	return false;
}

ValueNode::LooseHandle
ValueNode_Pow::get_link_vfunc(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return base_;
	if(i==1) return power_;
	if(i==2) return epsilon_;
	if(i==3) return infinite_;
	return 0;
}

int
ValueNode_Pow::link_count()const
{
	return 4;
}

String
ValueNode_Pow::link_local_name(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return _("Base");
	if(i==1) return _("Power");
	if(i==2) return _("Epsilon");
	if(i==3) return _("Infinite");
	return String();
}

String
ValueNode_Pow::link_name(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return "base";
	if(i==1) return "power";
	if(i==2) return "epsilon";
	if(i==3) return "infinite";
	return String();
}

int
ValueNode_Pow::get_link_index_from_name(const String &name)const
{
	if(name=="base")     return 0;
	if(name=="power")    return 1;
	if(name=="epsilon")  return 2;
	if(name=="infinite") return 3;

	throw Exception::BadLinkName(name);
}

ValueBase
ValueNode_Pow::operator()(Time t)const
{
	if (getenv("SYNFIG_DEBUG_VALUENODE_OPERATORS"))
		printf("%s:%d operator()\n", __FILE__, __LINE__);

	Real base     = (*base_)    (t).get(Real());
	Real power    = (*power_)   (t).get(Real());
	Real epsilon  = (*epsilon_) (t).get(Real());
	Real infinite = (*infinite_)(t).get(Real());



	if (epsilon < 0.00000001)
		epsilon = 0.00000001;

	//Filters for special/undefined cases

	if (abs(power) < epsilon) //x^0 = 1
		return 1;
	if (abs(base) < epsilon)
	{
		if (power > 0) //0^x=0
			return Real(0);
		else
		{
			if ( ( ((int) power) % 2 != 0) && (base < 0) ) //(-0)^(-odd)=-inf
				return -infinite;
			else
				return infinite;
		}
	}
	if (base <= epsilon && ((int) power) != power) //negative number to fractional power -> undefined
		power = ((int) power); 	//so round off power to nearest integer

	return pow(base,power);

}

String
ValueNode_Pow::get_name()const
{
	return "power";
}

String
ValueNode_Pow::get_local_name()const
{
	return _("Power");
}

bool
ValueNode_Pow::check_type(ValueBase::Type type)
{
	return type==ValueBase::TYPE_REAL;
}
