#pragma once

#ifndef MAIN_H
#define MAIN_H

#include <argp.h>
#include <string>

struct arguments
{
    char *args[1];
    bool restorehook, noinfistar, filepatching;
    char *sourcescript;
};

#endif // MAIN