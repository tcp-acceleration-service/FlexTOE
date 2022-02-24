#!/usr/bin/python

#
# Copyright (C) 2018,  Netronome Systems, Inc.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @file          me/blocks/gro/scripts/ucverify.py
#

import sys
import re

instr_pat = re.compile('^\.(\d+)')
ref_pat = re.compile('\.macro_ref\s+(\w+)')
endref_pat = re.compile('\.end_macro_ref\s+(\w+)')
verify_pat = re.compile(';\s*VERIFY:\s*icount\s+(\w+)\s+(\d+)')

cur_instr = 0
live_macros = []
tracked_macros = { }
lineno = 0
rc = 0

def expected_len(name):
    if tracked_macros.has_key(name):
        return tracked_macros[name]
    else:
        return -1

f = open(sys.argv[1])
for line in f.readlines():
    lineno += 1
    instr = instr_pat.search(line)
    if instr:
        cur_instr = int(instr.group(1))
        continue

    ref = ref_pat.search(line)
    if ref:
        macro = ref.group(1)
        live_macros.append((macro, lineno, cur_instr, expected_len(macro)))
        continue

    endref = endref_pat.search(line)
    if endref:
        macro = endref.group(1)
        (lastm, start_line, start_instr, expected) = live_macros.pop()
        if lastm != macro:
            raise Exception("macro mismatch: '%s' expected, '%s' found" % (last, macro))
        if expected > 0 and cur_instr - start_instr != expected:
            print "Macro '%s' on line %d' is supposed to be '%d' instructions but is actually '%d'" % \
                    (macro, start_line, expected, cur_instr - start_instr)
            rc += 1
        continue

    verify = verify_pat.search(line)
    if verify:
        tracked_macros[verify.group(1)] = int(verify.group(2))
        continue


if rc:
    print "%s: %d macros had invalid lengths" % (sys.argv[1], rc)
    sys.exit(1)
else:
    print "%s: All macros verified correctly" % sys.argv[1]
