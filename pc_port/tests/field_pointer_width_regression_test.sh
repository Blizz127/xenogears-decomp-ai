#!/usr/bin/env bash
# Retail field/model records contain 32-bit pointer slots.  On the LP64 host,
# reading or writing those slots through void* or u8** changes the record
# layout and consumes the adjacent word.  This focused source regression keeps
# the audited functions on explicit four-byte accesses while retaining
# independent source/destination actor indices.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
MODEL_SRC="$ROOT/src/slus_006.64/system/temp2.c"
FIELD_SRC="$ROOT/src/field/main/misc6.c"
FIELDLOAD_SRC="$ROOT/src/field/main/misc3.c"

test -f "$MODEL_SRC"
test -f "$FIELD_SRC"
test -f "$FIELDLOAD_SRC"

model_fn="$(awk '/^void func_8002C8CC\(/{on=1} /^void func_8002CB54\(/{on=0} on' "$MODEL_SRC")"
teardown_fn="$(awk '/^void func_8002CBBC\(/{on=1} /^extern s32 D_80059310/{on=0} on' "$MODEL_SRC")"
field_fn="$(awk '/^void func_800A0524\(/{on=1} /^extern void func_800A0C94/{on=0} on' "$FIELD_SRC")"
skeletal_init="$(awk '/^void\* func_800303C8\(/{on=1} /^#endif/{if(on){print;exit}} on' "$MODEL_SRC")"
skeletal_tick="$(awk '/^void func_800305D8\(/{on=1} /^#endif/{if(on){print;exit}} on' "$MODEL_SRC")"
fieldload_fn="$(awk '/^void FieldLoad\(/{on=1} /^#endif \/\* XENO_PC_PORT \*\//{on=0} on' "$FIELDLOAD_SRC")"

test -n "$model_fn"
test -n "$teardown_fn"
test -n "$field_fn"
test -n "$skeletal_init"
test -n "$skeletal_tick"
test -n "$fieldload_fn"

# func_8002C8CC's packed model slot +0x18 is a retail sw/lw slot.
! grep -Eq '\*\(void\*\*\)\(s0 \+ 0x18\)' <<<"$model_fn"
grep -Eq '\*\(u32\*\)\(s0 \+ 0x18\) = \(u32\)\(uintptr_t\)HeapAlloc' <<<"$model_fn"
! grep -Eq '\*\(void\*\*\).*0x18|0x18.*\*\(void\*\*\)' <<<"$teardown_fn"
grep -Fq 'HeapFree((void*)(uintptr_t)*(u32*)(modelData + 0x18));' <<<"$teardown_fn"

# func_800A0524's actor slots +0x4C/+0x04 are also four-byte guest pointers.
! grep -Eq '\*\(u8\*\*\)' <<<"$field_fn"
grep -Fq 'u8* pSrcData = (u8*)(uintptr_t)*(u32*)(pSrcField + 0x4C);' <<<"$field_fn"
grep -Fq 'u8* pDstData = (u8*)(uintptr_t)*(u32*)(pDstField + 0x4C);' <<<"$field_fn"
grep -Fq 'u8* pSrcSub = (u8*)(uintptr_t)*(u32*)(pSrcField + 0x4C);' <<<"$field_fn"
grep -Fq 'u8* pDstSprite = (u8*)(uintptr_t)*(u32*)(pDstField + 0x04);' <<<"$field_fn"

# Keep both caller-provided actor indices (including nonzero indices) and the
# retail 92-byte FieldActor stride visible in the implementation.
grep -Fq 'srcIdx * 92' <<<"$field_fn"
grep -Fq 'dstIdx * 92' <<<"$field_fn"

# Skeletal work-block / per-joint slots stay four bytes (func_800303C8 /
# func_800305D8). FieldLoad must publish the returned work block into
# pModel+0x14 as a u32; storing 0 (or a native pointer) drops animation.
! grep -Eq '\*\(void\*\*\)' <<<"$skeletal_init"
grep -Fq '*(u32*)(s3 + 0x0) = (u32)(uintptr_t)func_8003014C;' <<<"$skeletal_init"
! grep -Eq '\*\(u8\*\*\)|\*\(s16\*\*\)|\*\(s32 \(\*\*\)' <<<"$skeletal_tick"
grep -Fq '(u8*)(uintptr_t)*(u32*)(pAnimData + 0x00)' <<<"$skeletal_tick"
grep -Fq '(u8*)(uintptr_t)*(u32*)(pAnimData + 0x10)' <<<"$skeletal_tick"
grep -Fq 'pAnimInfo = func_800303C8(' <<<"$fieldload_fn"
grep -Fq '*(u32*)((u8*)pModel + 0x14) = (u32)(uintptr_t)pAnimInfo;' <<<"$fieldload_fn"
! grep -Eq '\*\(s32\*\)\(\(u8\*\)pModel \+ 0x14\) = 0;' <<<"$fieldload_fn"

echo 'field pointer-width containment and indexed actor semantics: PASS'
