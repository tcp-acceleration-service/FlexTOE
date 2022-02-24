/*
 * Copyright (C) 2014-2015,  Netronome Systems, Inc.  All rights reserved.
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
 * @file:   init_nbi_tm.uc
 * @brief:  Intermediate Pre-Classifier setup
 *
 */

#ifndef _INIT_NBI_PC_UC_
#define _INIT_NBI_PC_UC_

#ifndef NBI_PKT_PREPEND_BYTES
    #error "NBI_PKT_PREPEND_BYTES is not defined"
#endif

#include "init_nbi_pc_csr.uc"

/** Nbi_PreClassifier_Init
 *
 * Pre-Classifier initialisation
 *
 * @param NBI_ID        NBI number, can be 0 or 1
  */
#macro Nbi_PreClassifier_Init(NBI_ID)
    #if (NBI_ID < 0) || (NBI_ID > 1)
        #error "NBI_ID can only be 0 or 1"
    #endif

    /* NBI Packet Classifier Work around for A0 bug
          NFPBSP-1226
          NFPBSP-1179
    */
    #if ((__REVISION_MIN == __REVISION_A0) || \
         (__REVISION_MIN == __REVISION_A1))
        Nbi_PktPreclassifier_NFPBSP_1179(NBI_ID)
    #endif

    /* The PortCfg register is write-only, hence we must configure its
       fields in one write
    */
    #if (NBI_PKT_PREPEND_BYTES>0)
        #define_eval PORT_CFG_SKIP (NBI_PKT_PREPEND_BYTES/2)
    #else
        #define PORT_CFG_SKIP 0
    #endif
    #define ANALYSIS    1
    #define_eval PORT_CFG_VAL ((PORT_CFG_SKIP << 2) | (ANALYSIS))
    #define_eval PORT_CFG_LOOP (127)
    #while (PORT_CFG_LOOP >= 0)
        Nbi_PktPreclassifier_Characterization_PortCfg(NBI_ID,PORT_CFG_LOOP,
        PORT_CFG_VAL)
        #define_eval PORT_CFG_LOOP (PORT_CFG_LOOP-1)

    #endloop
    #undef ANALYSIS
    #undef PORT_CFG_SKIP
    #undef PORT_CFG_VAL
    #undef PORT_CFG_LOOP

#endm

#endif /* _INIT_NBI_PC_UC_ */
