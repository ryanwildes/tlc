#!/usr/bin/env lua
tlc = require "tlc"

program =
[[#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Hello, %s!\n", argv[1]);
    return 0;
}]]

assert(type(tlc) == "table")
ctx = tlc.new()
assert(type(ctx) == "userdata")

assert(ctx:setoutputtype(tlc.output.memory))
assert(ctx:compilestring(program))
assert(ctx:run(1, "world"))
assert(ctx:delete())
