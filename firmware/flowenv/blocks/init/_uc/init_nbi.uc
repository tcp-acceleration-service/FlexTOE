/*
 * Copyright (C) 2014-2018,  Netronome Systems, Inc.  All rights reserved.
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
 * @file:   init_nbi.uc
 * @brief:  Uppermost level of the NBI Island/s initialisation process
 *
 */

 /*
  * The abstraction of the NBI initialisation looks like this:
  *
  * Top most level. The nbi_init() calls the distinct setup macros for the DMA,
  * Traffic Manager (TM) and Preclassifier (PC).
  *     init_nbi.uc
  *
  * Intermediate level.
  *     init_nbi_dma.uc
  *     init_nbi_tm.uc
  *     init_nbi_pc.uc

  * Bottom level. All macros use the .init_csr directives to set the
  * registers.
  *     init_nbi_dma_csr.uc
  *     init_nbi_tm_csr.uc
  *     init_nbi_pc_csr.uc
  *
  */

#ifndef _INIT_NBI_UC_
#define _INIT_NBI_UC_

#include "../init_config.h"
#include "init_nbi_tm.uc"
#include "init_nbi_dma.uc"
#include "init_nbi_pc.uc"

/* Sanity Checks */
#ifndef NBI_COUNT
    #error "NBI_COUNT not defined"
#endif

#macro nbi_init()

    #define NBI_ID 0
    #while (NBI_ID < NBI_COUNT)

        Nbi_Dma_Init(NBI_ID)
        Nbi_PreClassifier_Init(NBI_ID)

        #define_eval NBI_ID (NBI_ID + 1)
    #endloop

    #ifndef NBI_TM_INIT_SKIP
    Nbi_TrafficManager_Init(NBI_COUNT)
    #endif

#endm

#endif /* _INIT_NBI_UC_ */
