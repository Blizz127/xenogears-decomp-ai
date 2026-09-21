# W34N19 — frame-driver authority and party refresh

## Result

`RESTORED_AND_CERTIFIED`. Retail's party refresh and menu/transition mode
lanes are now explicit production seams. Five signed-low address
transcriptions no longer select the wrong page or the wrong host/guest
authority.

Starting HEAD was `20871946921cd921766055680ff236b1bfb83606` on
`experiment/worldmap-open-gates-20260823`, equal to origin.

## Retail anchor

Authority is `disc/world_map.bin`, loaded at `0x8006FAF0`, SHA-256
`4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.

| range | role | file offset / length | slice SHA-256 |
|---|---|---|---|
| `0x800714C8..0x8007169C` | party refresh | `0x19D8 / 0x1D4` | `03d5a6a80ad6af64defba03060be2643f57c4aadecb290f1597f4789cd462596` |
| `0x80071890..0x80071974` | menu/transition dispatch | `0x1DA0 / 0xE4` | `8a98376f997b27280ed1e73d56f2c11c972f34281fc89ea421a99971494836f0` |
| `0x8007290C..0x8007293C` | common-tail prefix | `0x2E1C / 0x30` | `c4b7ef84d075e1fdeca2ecd17e36d2cf3a5cb3cf85d11a49720f268bfe6a3e1e` |

Signed low halves establish the exact retail addresses:

- `lui 0x8006; lbu/sb 0x9179/0x9460/0x9178/0x9171` resolves to
  `0x80059179`, `0x80059460`, `0x80059178`, and `0x80059171`, not the
  `0x80069xxx` addresses used before this rung. These are compiled native
  authorities (`D_80059179`, `D_80059460`, `g_MenuDebugEnabled`, and
  `D_80059171`).
- `lui 0x8007; addiu 0xEE68` resolves to guest `0x8006EE68`, not
  `0x8007EE68`.

The common-tail writer of `D_80059179` now uses the same native authority as
the frame driver, field code, and the already-certified W34N17 menu lifecycle.
It no longer updates an isolated guest twin.

## Corrected party lane

Source comparison found three independent defects beyond the page typo:

1. The old body inverted retail's `F8E5[0]` branch. Retail clears all three
   presence bytes when channel zero is nonzero; it rebuilds presence from the
   primary selectors when channel zero is zero.
2. Retail indexes selectors at guest `0x8006F368+i`, not the presence bytes at
   `0x8006F8E5+i`.
3. The character-table stride is `0xA4`, not `0x154`.

The exact guard is retained, the old presence snapshot is copied to
`0x8006EE70/72/74`, and `wm_80075D4C` is called only when the area result's
signed low half is not four. `BD34` is now cleared at the retail
reconvergence on both guard outcomes; the former approximation left it set
when the guard rejected.

The nearby fabricated `D804 != 0` / `BD10 & 0x800` toggle block was removed.
It was not retail's pause call at `0x80071704`; the real pause lane is the
separate `wm_8007634C` frontier.

## Certificate and runtime

`pc_port/tests/run_w34n19_frame_authority.sh` links the production driver and
common-tail code:

- O0, O2, and nonrecovering UBSan: PASS
- focused warnings: clean
- M1 guest common-tail twin: detected
- M2 wrong-page party guard: detected
- M3 guard-only `BD34` clear: detected
- M4 inverted presence branch: detected
- M5 wrong selector bytes: detected
- M6 wrong `0x154` stride: detected
- M7 missing `75D4C`: detected
- M8 guest menu globals: detected
- M9 wrong-page `EE68`: detected

W34C1 cadence remains PASS under O0/O2/UBSan with M1-M21 detected. W34N14
and W34N17 focused certificates remain fully green. The normal port reports
`LINK OK`.

The detached 120-frame accepted route reached its bounded exit and fulfilled
both capture requests. Digests remain exactly the standing baseline, as the
repaired conditional lanes are dormant on this route:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

## Next code frontier

Exact read-only decodes established the paired pause functions
`0x8007634C..0x80076594` and `0x80076594..0x800767D4`. Production-complete
integration must also retire their two bounded generated callees,
`GraphicsDrawPauseLetters` and `SoundMuteAllSpuChannels`; wiring the pause
functions while those visible/audio effects remain no-ops is not accepted.
