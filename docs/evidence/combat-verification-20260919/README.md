# Combat verification, 2026-09-19

Baseline: `2aa01b38` on `experiment/worldmap-open-gates-20260823`.
This continues the field-12 repair with normal New Game combat testing.
Raw logs, screenshots and the crash core are local in
`scratchpad/astra-combat-20260919/`; licensed payloads are not committed.

## Confirmed crash and repair

A fresh normal New Game loaded archive 0x10/file 3, passed the opening scripted
Gear battle, and reached field 15 via 14 → 13 → 1. No direct-field harness,
state writes, stat changes or loaded checkpoint were used. Before the encounter,
a read-only debugger observation measured Fei's party slot 0, HP 50/50 and MP
10/10. The field-15 encounter rendered Fei and enemies, then crashed before the
first player command.

Core 1036273 identifies `func_8008AA74` → `func_8008AA40` at main43.c:41.
The shared native selector `D_8005919C` held `0x8012a8c0`; the halfword at guest
`0x8012a8d4` was 1. The native helper tried to dereference host address
`0x8012a8d4`. The raw guest selector slot itself was zero: the interpreter's
shared-data mapping directs that word to native storage. Reading only the raw
guest selector would therefore be wrong for this call path.

The port-only branch translates KUSEG/KSEG0/KSEG1 bank addresses and retains
native bank references. Retail still reads the selector word followed by the
bank's unsigned halfword at +0x14, shifts it by 16 and ORs in the low argument
byte. The gate remains unchanged. No default sound, missing-bank fallback or
battle outcome bypass was introduced.

## Tests and byte authority

The command-sound test now builds with the same overlay RAM aliases as the
shipped native body. Its previous fixture substituted a low native pointer,
which could not reproduce a real guest-address bank. The revised fixture covers
all three guest aliases at the crash's bank offset, plus a native bank, 256 low
argument bytes, four high-word patterns, four bank IDs and three gate values.
It failed with SIGSEGV before the fix and passes 49,152 cases each at O0, O2 and
UBSan afterward. Gate, packed-ID and untranslated-pointer mutants are rejected.
The retail oracle executes the original disc instructions and captures the sound
API call; it does not substitute a reconstructed oracle function.

Fresh GCC 2.7.2/maspsx MIPS compilation preserved the entire TU's `.text` bytes
(SHA256 `9c81b052326803b5ab61ae43ee9343139348ea556b886f85edb648b20d822e03`).
After resolving relocations to retail symbol addresses, `8008AA40` (52 bytes)
and `8008AA74` (44 bytes) each compare byte for byte with `disc/battle.bin`.
See `sound-bytes.json`. A flipped-word comparison is rejected.

Native build passes, including the gate that all 92 adopted leaves are linked
and reach none of the 82 generated function stubs. The 92-leaf differential
suite again passes O0/O2/UBSan and its wrong-result mutation. Its reported 380
inconclusive cases and eight pointer-parameter leaves remain explicit coverage
limits; a general sweep passing did not prevent the sound-pointer crash.

Six existing test suites were repaired without relaxing their assertions:

- Graphics ABI and guest-call tests link the production vblank service.
- Heap/audio reservation uses production-owned globals instead of duplicate definitions.
- Billboard binding expects the now-implemented retail type-8 callback, verified
  against the executable's pointer table at 0x8004FD40.
- Sprite-hook map generation uses native ELF function classification, as the
  production build does; addresses remain from the retail symbol maps.
- Battle-return extraction retains the production absolute-access aliases;
  the font fixture accepts and records the eleventh argument, and nine semantic
  mutations follow the current `next` variable and aliases. All 384 retail
  differential cases pass in each mode, with all nine mutations rejected.

The broader sweep ran 62 existing battle-related runners: 36 pass after the
repairs and an extended eligibility-RAM run (163,472 cases per mode plus bounds
and six mutations). The first aggregate run's 240-second timeout was too short
for that suite. The other 26 remain failing: obsolete bootstrap configuration
expectations or compile/link failures in older source/fixture wiring. These are
not silently skipped or counted as passes. Per-run status and log hashes are
recorded in `tests.json`. This is not certification of all combat code or all
old test suites.

## Runtime acceptance

Fixed binary SHA256:
`99f5b5780945d070c555e8f867c0d3e6c32473d9336fb020fdb0bf5fcb2e33de`.
Fresh normal New Game replay on this binary reached the mountain via ordinary
inputs. The first playable encounter (Jackal + two Hobs) passed command selection,
target selection, heavy attacks, enemy defeats, victory, EXP tally and item/gold
rewards. Fei finished at 50/50 HP and 10/10 MP, total EXP 7, one Hob-Jerky gained,
0 gold gained (total 100). The battle returned to field 15 at (585,-38,-726),
`canRun=1`, and directional inputs moved Fei again. This is an observed normal
playthrough, not a debugger-forced win or a field-test harness.

The existing user quicksave is preserved. The replay used a separate
`XENO_QUICKSAVE_PATH`; its precombat and postvictory checkpoints were earned and
saved with the game UI. Neither checkpoint was loaded to establish this win.
Local screenshots: `fixed-command-menu.png`, `victory-exp.png`,
`victory-tallied.png`, `victory-items.png`, `victory-field-return.png`.

The second encounter (single Jackal) also completed normally. Enemy turns
reduced HP from 50 to 49, then 45 and 44 during command/escape attempts. Escape
ultimately succeeded and returned to field 15 at (591,50,-1246), `canRun=1`.
The final read-only state check confirms HP 41/50, MP 10/10 and map 15.
`escape-field-return.png` and `postescape-state.log` retain that observation.
No checkpoint load or state patch was used in either encounter.

Exhaustive combat, later bosses, full audiovisual retail parity and the 26
legacy test failures remain outside this acceptance.

The owned game and virtual display were stopped after acceptance. No input driver
or combat test remains running. No push was performed.
