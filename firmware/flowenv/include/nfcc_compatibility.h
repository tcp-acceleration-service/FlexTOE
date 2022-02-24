/*
 * Copyright (C) 2018,  Netronome Systems, Inc.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          include/nfcc_compatibility.h
 * @brief         Fixed width memory address definitions
 */

#ifndef _NFCC_COMPATIBILITY_H_
#define _NFCC_COMPATIBILITY_H_

#include <nfp.h>

#ifndef __mem32
#define __mem32         __declspec(addr32 mem)
#endif

#ifndef __mem40
#define __mem40         __declspec(addr40 mem)
#endif

#ifndef __cls32
#define __cls32         __declspec(addr32 cls)
#endif

#ifndef __cls40
#define __cls40         __declspec(addr40 cls)
#endif

#ifndef __ctm32
#define __ctm32         __declspec(addr32 ctm)
#endif

#ifndef __ctm40
#define __ctm40         __declspec(addr40 ctm)
#endif


#endif /* END _NFCC_COMPATIBILITY_H_*/
