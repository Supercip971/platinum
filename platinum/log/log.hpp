#pragma once
#include <format>
#include <iostream>
#include <stdio.h>
namespace plat
{
struct Log
{
    static void info(const char *msg) { std::cout << msg << std::endl; }
    static void error(const char *msg) { std::cerr << msg << std::endl; }
}; // namespace plat
}; // namespace plat
