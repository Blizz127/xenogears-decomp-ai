#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
ulimit -c 0
out=$(mktemp -d scratchpad/rotation-zyx-retail.XXXXXX)
echo "Evidence: $out"
read -r pin _ < <(sha256sum disc/SLUS_006.64)
test "$pin" = dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119
sha256sum pc_port/src/retail_leaf_adapters.c pc_port/include_shim/psyq/libgte.h \
    pc_port/src/battle_mips_adapter.c pc_port/tests/rotation_zyx_retail_test.c \
    pc_port/extern/PsyCross/src/psx/LIBGTE.C \
    pc_port/extern/PsyCross/src/gte/rcossin_tbl.h > "$out/source-pins.txt"
common=(-fno-pie -DUSE_EXTENDED_PRIM_POINTERS=0 -ffunction-sections -fdata-sections
    -Ipc_port/include_shim -Iinclude -Ipc_port/src
    -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
for mode in O0 O2 UBSan; do
    flags=(-"$mode"); if [ "$mode" = UBSan ]; then flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all); fi
    for source in psx/LIBGTE.C psx/INLINE_C.C gte/PsyX_GTE.cpp; do
        name=${source##*/}
        g++ -std=c++17 "${common[@]}" "${flags[@]}" -fpermissive -w \
            -include pc_port/src/port_compat.h -c "pc_port/extern/PsyCross/src/$source" -o "$out/$mode.$name.o"
    done
    for source in pc_port/tests/rotation_zyx_retail_test.c pc_port/src/battle_mips_adapter.c pc_port/src/retail_leaf_adapters.c; do
        name=${source##*/}
        cc -std=gnu17 "${common[@]}" "${flags[@]}" -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
            -include assert.h -Wall -Wextra -Werror -c "$source" -o "$out/$mode.$name.o"
    done
    clang++ -no-pie "${flags[@]}" -Wl,--gc-sections "$out/$mode."*.o -o "$out/$mode"
    "$out/$mode" 2>"$out/$mode.stderr" | tee "$out/$mode.log" || { cat "$out/$mode.stderr"; exit 1; }
done
perl - "$out" <<'PERL'
use strict; use warnings;
open my $f,'<','pc_port/src/retail_leaf_adapters.c' or die $!; local $/; my $s=<$f>;
my @controls=(
 ['baseline','','',''],
 ['combined-rounding','RotationZYXProduct(sxsy, cz) - RotationZYXProduct(sz, cx)','((sxsy * cz - sz * cx) >> 12)','matrix/input/guards'],
 ['sine-sign','matrix->m[2][0] = -sy;','matrix->m[2][0] = sy;','matrix/input/guards'],
 ['missing-store','matrix->m[2][1] = RotationZYXProduct(sx, cy);','/* missing matrix store */','matrix/input/guards'],
 ['translation-write','return matrix;','matrix->t[0] = 0; return matrix;','matrix/input/guards'],
 ['gte-write','return matrix;','CTC2(0, 0); return matrix;','GTE unchanged'],
 ['wrong-return','return matrix;','return 0;','native return']
);
my ($body)=$s =~ /(MATRIX\* RotMatrixZYX\(SVECTOR\* rotation, MATRIX\* matrix\)\n\{.*?\n\})/s or die 'missing body';
for my $c (@controls) {
 my ($name,$old,$new,$reason)=@$c; my $b=$body;
 if(length $old) { my $n=($b =~ s/\Q$old\E/$new/g); die "$name replacements=$n" unless $n==1; }
 my $copy=$s; $copy =~ s/\Q$body\E/$b/ or die 'replace body';
 open my $o,'>',"$ARGV[0]/$name.c" or die $!; print $o $copy; close $o;
}
my $legacy=$s;
my $delegate="MATRIX* RotMatrixZYX(SVECTOR* rotation, MATRIX* matrix)\n{\n extern MATRIX* RotMatrixZYX_gte(SVECTOR*, MATRIX*);\n return RotMatrixZYX_gte(rotation, matrix);\n}";
$legacy =~ s/\Q$body\E/$delegate/ or die 'restore legacy delegate';
open my $o,'>',"$ARGV[0]/legacy-delegate.c" or die $!; print $o $legacy; close $o;
PERL
for control in baseline combined-rounding sine-sign missing-store translation-write gte-write wrong-return legacy-delegate; do
    cc -std=gnu17 "${common[@]}" -O2 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
        -include assert.h -Wall -Wextra -Werror -Wno-unused-function \
        -c "$out/$control.c" -o "$out/$control.o"
    clang++ -no-pie -O2 -Wl,--gc-sections "$out/$control.o" \
        "$out/O2.rotation_zyx_retail_test.c.o" "$out/O2.battle_mips_adapter.c.o" \
        "$out/O2.LIBGTE.C.o" "$out/O2.INLINE_C.C.o" "$out/O2.PsyX_GTE.cpp.o" -o "$out/$control"
    rc=0
    "$out/$control" > "$out/$control.log" 2>&1 || rc=$?
    if [ "$control" = baseline ]; then
        test "$rc" = 0; cat "$out/$control.log"
    else
        case "$control" in
            gte-write) reason='GTE unchanged';;
            wrong-return) reason='native return';;
            *) reason='matrix/input/guards';;
        esac
        test "$rc" = 1; rg -q "^ROT ZYX FAIL case=[0-9]+ $reason$" "$out/$control.log"
        echo "ROT ZYX control rejected: $control"
    fi
done
