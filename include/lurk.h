// license
// ---------------------------------------------------------------------------------------------- //
// Copyright (c) 2023, Casey Walker
// All rights reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//
//
// lurk.h
// ---------------------------------------------------------------------------------------------- //
// lurking...

#ifndef LURK_H
#define LURK_H

#define LURK_STRINGIZE(x) #x
#define LURK_DEFLECT_STRINGIZE(x) LURK_STRINGIZE(x)
#define LURK_LINE_STRING LURK_DEFLECT_STRINGIZE(__LINE__)
#define LURK_STR_SPACECAT(str0, str1) str0" "str1

#include "result.h"

#endif // LURK_H

