#include "foobar.h"
#include <iostream>

foobar::foobar()
    : test("aaaaa")
{
}

void foobar::getTest()
{

    std::cout << test;
}
