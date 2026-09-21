/* Battle overlay functions the port may run as native C instead of
 * interpreting disc/battle.bin.  Hand-maintained; add a name here, then run
 * pc_port/build_port.sh and pc_port/tests/run_battle_overlay_host_differential_test.sh.
 *
 * DO NOT ADOPT the functions below.  Each was tried and REJECTED by the
 * differential prover: its existing C body does not match the retail bytes.
 * They are latent today only because the interpreter still runs those paths.
 * Fix the body and prove it before adding any of them.  Names are left
 * unquoted deliberately -- battle_overlay_host_tus.py scrapes "func_..."
 * string literals out of this file, so a quoted name in a comment would be
 * read as an adopted leaf.
 *
 *   func_800BEDE8  mainc124.c  RAM: 2 bytes differ at guest 800c3612 (retail 00, host 1e)
 *   func_800AA7DC  main71.c    faults where the interpreter completes; also v0 retail 27 vs host 0
 *   func_800AA79C  main71.c    faults where the interpreter completes (write8 at 0x34, pc 800aa7b8)
 *   func_8008887C  main40.c    RAM: 1 byte differs at guest 800c3a9c (retail 3e, host 32)
 *   func_800885D0  main39.c    rejected 2026-09-21 once main39.c became compilable
 *   func_800AA760  main70.c    rejected 2026-09-21 once main70.c became compilable
 *
 * Both main71.c entries fail the same way, which suggests one shared cause.
 *
 * UNPROVEN (not wrong, but the sweep cannot construct a conclusive case, so
 * adoption would be unverified): func_8008AA40, func_8008AB94 -- both hit
 * "unresolved native call target=0x80028470".
 */
    "func_80079934",
    "func_80089B50",
    "func_800A3484",
    "func_800AEEEC",
    "func_800B16A4",
    "func_800B6930",
    "func_800B6990",
    "func_800B69E4",
    "func_800AA898",
    "func_80079ED8",
    "func_8007A280",
    "func_80079E18",
    "func_80079E4C",
    "func_80079054",
    "func_80079098",
    "func_80079114",
    "func_80079E7C",
    "func_8007A628",
    "func_8007A6C8",
    "func_8007A744",
    "func_8007BA88",
    "func_8007BAB8",
    "func_8007E674",
    "func_8007EED0",
    "func_800BF720",
    "func_8007EEE8",
    "func_8007FCE8",
    "func_80089C08",
    "func_8008AAA0",
    "func_8008B108",
    "func_8008BC40",
    "func_80089BEC",
    "func_800A577C",
    "func_8007EF44",
    "func_8008CCCC",
    "func_8009C0E0",
    "func_8009E268",
    "func_800A578C",
    "func_800A5E9C",
    "func_80077980",
    "func_800AF400",
    "func_800B14B8",
    "func_800B8048",
    "func_800B8054",
    "func_800BC3F8",
    "func_800BC454",
    "func_800BCD8C",
    "func_800BE108",
    "func_800BED30",
    "func_800BF5E8",
    "func_800BF730",
    "func_800BF954",
    "func_800C0F70",
    "func_800B7C28",
    "func_800B89F4",
    "func_80071A08",
    "func_800BF6F8",
    "func_80080AE4",
    "func_80080B64",
    "func_80080C6C",
    "func_80085E78",
    "func_800879A8",
    "func_80088990",
    "func_80089C24",
    "func_80089C48",
    "func_80089C6C",
    "func_80089C9C",
    "func_80099FB0",
    "func_8009A074",
    "func_8009B098",
    "func_8009E364",
    "func_8009E410",
    "func_8009E48C",
    "func_8009E508",
    "func_8007887C",
    "func_80078D48",
    "func_800B3B6C",
    "func_8008AA74",
    "func_8009CB68",
    "func_8009CA90",
    "func_800B9B30",
    "func_8009795C",
    "func_8009F5B0",
    "func_800B3348",
    "func_800B3350",
    "func_800BAF40",
    "func_800BDD34",
    "func_80097D08",
    "func_8009C050",
    "func_800B00D0",
    "func_800BCB54",
    "func_800BF0B4",
    "func_800A3490",
    "func_800A3514",
    "func_800A3578",
    "func_800A35C8",
    "func_80085388",
    "func_800800E8",
    "func_80085454",
    "func_80085AC4",
    "func_8009AA44",
    "func_8009AB00",
    "func_800AA788",
