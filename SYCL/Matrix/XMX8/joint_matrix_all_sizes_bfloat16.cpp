//==-------- joint_matrix_all_sizes.cpp  - DPC++ joint_matrix---------------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// REQUIRES: matrix-xmx8

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_ALL_BF16
// RUN: %CPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_1
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_2
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_3
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_4
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_5
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_6
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_7
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

// RUN: %clangxx -fsycl %s -o %t.out -DSYCL_EXT_ONEAPI_MATRIX_VERSION=4 -DJOINT_MATRIX_TEST_BF16_8
// RUN: %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=0 %GPU_RUN_PLACEHOLDER %t.out
// RUN: env IGC_JointMatrixLoadStoreOpt=1 %GPU_RUN_PLACEHOLDER %t.out

#include <iostream>
#include <sycl/sycl.hpp>

using namespace sycl;
using namespace sycl::ext::oneapi::experimental::matrix;
using bfloat16 = sycl::ext::oneapi::bfloat16;

#define SG_SZ 8

#include "../joint_matrix_all_sizes_impl.hpp"
