cmake_minimum_required(VERSION 3.13)
project(compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(compiler
        header/errno.h
        header/global.h
        src/lexer.c
        header/lexer.h
        src/logger.c
        header/logger.h
        src/main.c src/parser.c header/parser.h src/gen.c header/gen.h src/hashtable.c header/hashtable.h)
