# W34C8 — provenance of the camera inputs (read-only)

Branch `experiment/worldmap-open-gates-20260823`, base `7829732a`. gdb
reads and retail disassembly only. Artifacts beside W34C7's
(`scratchpad/w34c7_triangle_order.*`: `w34c8_camera_inputs.txt`,
`w34c8_seed.gdb`, `retail_7565c.txt`).

## Observed (frames 1 / 30 / 60, forced route)

| field | frame 1 | frame 60 | side |
|---|---|---|---|
| `0x8009D3F0` camera height | `0x00460000` | `0x00460000` | source for `96F18` |
| `0x8009BE28..30` camera position | `(0,0,0)` | `(0,0,0)` | source for `96F18` |
| `0x8009BD38..3C` angles | `(-608, 3584, 0)` | same | source for `96F18`/`97440` |
| `0x8009BD40` record pos/target | zeros | `(471,-900,-472)/(0,0,0)` | producer output of `96F18` |
| `0x8009BBB4` world position | `(0,0,0)` | `(0,0,0)` | source for `980D4`/`981C8` |
| `0x8009D144` selector | 0 | 0 | 71A58 takes the `97440` branch |

`wm_80096F18` is retail-faithful (`[0x80096F18,0x80097070)`: `pos = target +
R·(0,0,-(height>>12))`, `target = (0, posY>>12, 0)`, `up = R·(0,-4096,0)` at
+0x10); its output places the eye ~1000 units from the world origin, 900
units up, with pitch −608 — inside the ±1024 range of authored tile heights.

## Retail producers (rule 1)

Store census over the overlay for the three inputs: ~40 sites. The one on
the world-entry path is **`0x8007565C`** (leaf, `[0x8007565C,0x800758C0)`,
153 instructions, no callees), called at `0x80072424` from the entry
sequence `0x80072238`. Its absolute store set (56 targets) includes
`0x8009BE28/2C/30` (`0x80075888-90`), `0x8009BD38..3F` (`0x80075804-10`,
swl/swr), `0x8009D3F0` (`0x800758AC`), `0x8009BBB4..C0`, `0x8009C5AC..B8`
(runtime X/Y/Z), `0x8009C838/3C` (map), `0x8009C854..73`, `0x8009CEC4..D0`,
`0x8009D55C..68`, `0x800A0000..0C`. Per-frame updates after entry come from
callback `91c18` (`0x80091B54/0x80091C18`), which the port has.

## Port

`world_map_init.c` transcribes the entry sequence as slices W2…W34B5E, but
jumps from W8B (`0x80098044` @ `0x80072378`, cut before `0x80072380`) to W10A
(`0x8008440C` @ `0x8007243C`). Retail `[0x80072380,0x8007243C)` —
`0x8001B66C`, `0x80039CC4`, `0x800399D4`, `0x80073398`, `0x80073448`,
**`0x8007565C`**, `0x80075D4C`, `ArchiveCdDataSync` — is not transcribed.
The only port writers of the three inputs are in `world_map_callback_91c18.c`
(`D3F0`: `0x400000` on sub-state 0x11, then interpolated toward
`slot+0x54`; `BD38`: interpolated toward `slot+0x58`); nothing writes
`BE28`. (Correction to W34C2's note: `0x80084580` *is* transcribed, as
W11B.)

## Verdict

`CAMERA_SEED_UNPORTED`: the frame-60 eye placement is produced by `91c18`'s
per-frame interpolation from an unseeded state, not by retail's entry seed
`0x8007565C`. Whether retail's seed would place the eye far enough above the
terrain to make the frame-60 field read as terrain is not measured here.

## Limitations

- No retail frame-60 reference; "comparable to retail" is not claimed.
- `0x8007565C`'s 56 store targets are listed, not decoded; several
  (`0x8009C854..73`, `0x800A0000..0C`) have no field-map entry yet.
- `91c18`'s own fidelity to `0x80091C18` was not re-verified this rung.

## Next (one task)

W34C9 — transcribe `0x8007565C` (leaf, 153 insns) into the init sequence at
its retail position (after `0x80073448`, before `0x80075D4C`), decoding its
store set with PCs into the field map, with a certificate whose oracle is
the retail store contract; then re-dump this table and re-capture frame 60.
The neighbouring untranscribed calls in `[0x80072380,0x8007243C)` should be
listed with sizes in the same rung, not ported.
