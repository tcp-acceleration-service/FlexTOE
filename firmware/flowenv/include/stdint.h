/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          stdint.h
 * @brief         C99 Integer types
 */

#ifndef _STDINT_H_
#define _STDINT_H_

/* Integral types */
typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef long long int           int64_t;

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

/* Limits of integral types */
#define INT8_MIN                (-128)
#define INT16_MIN               (-32767-1)
#define INT32_MIN               (-2147483647-1)

#define INT8_MAX                (127)
#define INT16_MAX               (32767)
#define INT32_MAX               (2147483647)

#define UINT8_MAX               (255)
#define UINT16_MAX              (65535)
#define UINT32_MAX              (4294967295U)

#endif /* _STDINT_H_ */
