#include <fmt/core.h>
#include <iostream>
#include <platinum/log/log.hpp>
#include <platinum/utility/result.hpp>

plat::Result<int> test(int v)
{
    if (v == 2)
    {
        return (v);
    }

    return plat::Result<int>::err("test failed");
}

plat::Result<char *> test2(int c)
{
    try$(test(c));

    return (char *)"test2";
}
int main(int argc, char **argv)
{

    debug$("hello world {} {}!", 1, 2);
    info$("info");
    warn$("warn");
    error$("error");
    fatal$("fatal");
    return 0;
}
