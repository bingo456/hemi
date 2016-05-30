///////////////////////////////////////////////////////////////////////////////
// 
// "Hemi" CUDA Portable C/C++ Utilities
// 
// Copyright 2012-2015 NVIDIA Corporation
//
// License: BSD License, see LICENSE file in Hemi home directory
//
// The home for Hemi is https://github.com/harrism/hemi
//
///////////////////////////////////////////////////////////////////////////////
// Please see the file README.md (https://github.com/harrism/hemi/README.md) 
// for full documentation and discussion.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "kernel.h"

#ifdef HEMI_CUDA_COMPILER
#include "configure.h"
#endif

namespace hemi {
//
// Automatic Launch functions for closures (functor or lambda)
//
template <typename Function, typename... Arguments>
ExecutionPolicy launch(Function f, Arguments... args)
{
#ifdef HEMI_CUDA_COMPILER
    ExecutionPolicy p;
    launch(p, f, args...);
    return p;
#else
    Kernel(f, args...);
    return ExecutionPolicy();
#endif
}

//
// Launch with explicit (or partial) configuration
//
template <typename Function, typename... Arguments>
#ifdef HEMI_CUDA_COMPILER
ExecutionPolicy launch(const ExecutionPolicy &policy, Function f, Arguments... args)
{
    ExecutionPolicy p = policy;
    checkCuda(configureGrid(p, Kernel<Function, Arguments...>));
    Kernel<<<p.getGridSize(), 
             p.getBlockSize(), 
             p.getSharedMemBytes(), 
             p.getStream()>>>(f, args...);
    return p;
}
#else
ExecutionPolicy launch(const ExecutionPolicy&, Function f, Arguments... args)
{
    Kernel(f, args...);
    return ExecutionPolicy();
}
#endif

//
// Automatic launch functions for __global__ kernel function pointers: CUDA only
//

template <typename... Arguments>
ExecutionPolicy cudaLaunch(void(*f)(Arguments... args), Arguments... args)
{
#ifdef HEMI_CUDA_COMPILER
    ExecutionPolicy p;
    cudaLaunch(p, f, args...);
    return p;
#else
    f(args...);
    return ExecutionPolicy();
#endif
}

//
// Launch __global__ kernel function with explicit configuration
//
template <typename... Arguments>
#ifdef HEMI_CUDA_COMPILER
ExecutionPolicy cudaLaunch(const ExecutionPolicy &policy, void (*f)(Arguments...), Arguments... args)
{
    ExecutionPolicy p = policy;
    checkCuda(configureGrid(p, f));
    f<<<p.getGridSize(), 
        p.getBlockSize(), 
        p.getSharedMemBytes(),
        p.getStream()>>>(args...);
    return p;
}
#else
ExecutionPolicy cudaLaunch(const ExecutionPolicy&, void (*f)(Arguments...), Arguments... args)
{
    f(args...);
    return ExecutionPolicy();
}
#endif

} // namespace hemi
