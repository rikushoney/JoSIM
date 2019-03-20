// Copyright (c) 2019 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)
#ifndef J_STD_INCLUDE
#define J_STD_INCLUDE

// Version info
#define VERSION 2.2

// Executable info
#ifndef WIN32
#define EXEC josim.exe
#else
#define EXEC josim
#endif

// Standard library includes
#include <algorithm>
#include <cmath>
#include <float.h>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <exception>
#include <ctime>
#ifdef _OPENMP
#include <omp.h>
#endif

// Linear algebra include
#include "klu.h"

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1,T2> &p) const noexcept
    {
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

#include "j_globals.h"

#endif
