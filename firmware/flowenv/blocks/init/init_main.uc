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
 * @file:  init_main.uc
 * @brief: Global NFP initialisation settings
 *
 * @note   The list file output of this source file is not intended to be
 *         loaded on any particular ME. During the build process the
 *         application can (via the NFCC -L flag) link/load this
 *         instruction-less list file.
 *
 */
#include "_uc/init_nbi.uc"

.begin
    nbi_init()
.end
