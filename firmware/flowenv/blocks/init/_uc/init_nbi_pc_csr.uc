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
 * @file:   init_nbi_pc_csr.uc
 * @brief:  Contains macros to initialise the NBI Packet Pre-Classifier
 */

#ifndef _INIT_NBI_PC_CSR_UC_
#define _INIT_NBI_PC_CSR_UC_

 /** Nbi_PktPreclassifier_Characterization_PortCfg
 *
 * NBI Pre-Classifier Port configuration. See the NFP-6xxx Databook section on
 * the NBI Packet Pre classifier
 *
 * @param NBI_ID    The NBI number, can be 0 or 1
 * @param PORT      The port number
 * @param VALUE     Method of interpreting the SRAM data read, to
 *                  produce a result for the lookup
 */
#macro Nbi_PktPreclassifier_Characterization_PortCfg(NBI_ID,PORT,VALUE)
    .init_csr xpb:Nbi/**/NBI_ID/**/IsldXpbMap.NbiTopXpbMap.PktPreclassifier.Characterization.PortCfg/**/PORT VALUE const
#endm


 /** Nbi_PktPreclassifier_Characterization_PortCfg
 *
 * NBI Pre-Classifier Work-around values for the following bugs:
 *        NFPBSP-1226
 *        NFPBSP-1179
 *
 * @param NBI_ID    The NBI number, can be 0 or 1
 */
#macro Nbi_PktPreclassifier_NFPBSP_1179(NBI_ID)
    #if (NBI_ID == 0)
        .init_csr xpbm:0x0048290000 0x32ff0000 32 const
    #else
        .init_csr xpbm:0x0049290000 0x32ff0000 32 const
    #endif
#endm

#endif /* _INIT_NBI_PC_CSR_UC_ */
