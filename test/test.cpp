// test.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "test.h"


// This is an example of an exported variable
TEST_API int ntest=0;

// This is an example of an exported function.
TEST_API int fntest(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see test.h for the class definition
Ctest::Ctest()
{
	return;
}
