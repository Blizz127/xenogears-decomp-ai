# Build the world map's matched C (src/world_map/main.c) the way the port
# compiles game TUs, for runners that exercise a switched func_XXXXXXXX
# instead of a retired wm_XXXXXXXX body.  The matched bodies read world-map
# data through pc_port/src/world_map_port_data.h (PSX_ADDR into g_PsxRam), so
# the test must define g_PsxRam.
#   wm_obj=$(world_map_matched_obj "$OUT")
world_map_matched_obj() {
    local out="$1" root
    root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
    mkdir -p "$out"
    "${CC_BIN:-${CC:-cc}}" -std=gnu17 -w -fpermissive -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
        -include assert.h -fno-builtin -O0 -g \
        -I"$root/pc_port/include_shim" -I"$root/include" \
        -I"$root/pc_port/extern/PsyCross/include" -I"$root/pc_port/extern/PsyCross/include/psx" \
        -I"$root/pc_port/src" \
        -c "$root/src/world_map/main.c" -o "$out/world_map_matched_main.o" >&2
    # Weak stand-ins for the matched bodies' port-side callees (adapters and
    # PsyCross), so focused runners link without the whole port.  They abort
    # if a test ever reaches them; a real definition, when linked, wins.
    cat > "$out/world_map_matched_callees.c" <<'CALLEES'
#include <stdlib.h>
__attribute__((weak)) void func_8008BFD4(int a, void* b, int c, int d) { (void)a; (void)b; (void)c; (void)d; abort(); }
__attribute__((weak)) void func_80087904(void* a, int b, unsigned short c, int d) { (void)a; (void)b; (void)c; (void)d; abort(); }
__attribute__((weak)) void* RotMatrix(void* r, void* m) { (void)r; (void)m; abort(); }
CALLEES
    "${CC_BIN:-${CC:-cc}}" -std=gnu17 -O0 -c "$out/world_map_matched_callees.c" -o "$out/world_map_matched_callees.o" >&2
    ld -r -o "$out/world_map_matched.o" "$out/world_map_matched_main.o" "$out/world_map_matched_callees.o" >&2
    echo "$out/world_map_matched.o"
}
