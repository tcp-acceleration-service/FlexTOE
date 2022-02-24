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
 * @file          me/blocks/gro/_uc/gro_internal.h
 */

#ifndef __GRO_INTERNAL_UC
#define __GRO_INTERNAL_UC

#include <gro.h>

#define GRO_NN_CTX_shf  2
#define GRO_NN_CTX_msk  ((GRO_ROCTX_PER_BLOCK - 1) << GRO_NN_CTX_shf)
#define GRO_NN_NREL_shf 16
#define GRO_NN_NREL_msk (GRO_MAX_REL_PER_MSG - 1)



#endif /* __GRO_INTERNAL_UC */
