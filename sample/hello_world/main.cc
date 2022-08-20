#include <fmt/core.h>
#include <iostream>
#include <platinum/log/log.hpp>

int main(int argc, char **argv)
{
    debug$("hello world {} {}!", 1, 2);
    info$("info");
    warn$("warn");
    error$("error");
    fatal$("fatal");
    return 0;
}