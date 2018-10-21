//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

int global;

int
get_global() noexcept
	{
	return global;
	}
template <int A>
struct
info
	{
	int value;
	};
void
f0()
	{
	auto propagate = leaf::preload( info<0>{0} );
	}
leaf::error
f1()
	{
	f0();
	global = 0;
	auto propagate = leaf::preload( info<1>{1}, [ ] { return info<42>{get_global()}; } );
	global = 42;
	return leaf::error( info<2>{2} );
	}
leaf::error
f2()
	{
	auto propagate = leaf::preload( info<3>{3} );
	propagate.cancel();
	return f1().propagate( info<4>{4} );
	}
int
main()
	{
	leaf::expect<info<0>,info<1>,info<2>,info<3>,info<4>,info<42>> exp;
	leaf::error e = f2();
	BOOST_TEST(!leaf::peek<info<3>>(exp,e));
	int c=0;
	handle_error( exp, e,
		leaf::match<info<42>,info<1>,info<2>,info<4>>( [&c]( int i42, int i1, int i2, int i4 )
			{
			BOOST_TEST(i42==42);
			BOOST_TEST(i1==1);
			BOOST_TEST(i2==2);
			BOOST_TEST(i4==4);
			++c;
			} ) );
	BOOST_TEST(c==1);
	return boost::report_errors();
	}
