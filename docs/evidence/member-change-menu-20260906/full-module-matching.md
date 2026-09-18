# Final `func_801C95A0` and member-menu matching check

Date: 2026-09-06

The private matching checkout received only the frozen shared
`src/member_change_menu/main/misc.c`, SHA-256
`15a4091854071fcb92c75236165cf836d4ce0a2dea0d47c822d7a0dd616a02cb`.
The prior private source was preserved as `pre-final95a0-private-misc.c`, SHA
`97d296853cf835816d0259dd1e10c834f7f2bb4550911753381e2988841f394e`.

The fresh isolated container run completed all 472/472 tasks. The checksum
gate exited 2 with exactly three remaining global reds. Log:
`container-final95a0-make-check.log`, SHA-256
`dc3b440aa46c2a1e6cf458f24267b7121d1d9d576516d0bd3009482aa35b7fce`.

| artifact | generated SHA-256 | retail SHA-256 | result |
|---|---|---|---|
| `slus_006.64` | `2eb8cb4bdba6aed54e1f0734bfb9c72d702ab5ee830139a58e70b432237cacca` | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` | red |
| `field.bin` | `ac772e7e9e8feb5dd73856a5bf64980b4730f373ef90303eac021805551283d8` | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` | red |
| `member_change_menu.bin` | `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` | `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` | exact |
| `shop_menu.bin` | `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` | `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` | red |

## Exact member-menu ranges

The map places the repaired functions at the retail VMAs:

```text
func_801C59E0                    0x801C59E0
func_801C5B90                    0x801C5B90
func_801C95A0                    0x801C95A0
MemberChangeMenuSwapCharacters   0x801CAB48
MemberChangeMenuMainLoop         0x801CAD14
```

Direct generated-versus-retail comparisons all pass:

| function | module range | size | exact SHA-256 |
|---|---:|---:|---|
| `func_801C59E0` | `0x09E0..0x0B8F` | 432 | `c88fecbc1a9c3f3eaf902986cde424f2ab5e807ec84b99dbbbc07ac7dc385c05` |
| `func_801C5B90` | `0x0B90..0x0BEB` | 92 | `9034aa4bc3bfa5b85de5c5d6729b1809b9eea849ec2a543a6375272853639a5b` |
| `func_801C95A0` | `0x45A0..0x469B` | 252 | `29febe445880a710e2005e11daed43370fa7b9db27f7fe32b778a7a240ef76c7` |
| `MemberChangeMenuSwapCharacters` | `0x5B48..0x5D13` | 460 | `a65fe1e11ad3f13b0cd9b732ec90a9a794ba476ef8b09b14ddf0f838a177fbd5` |

The entire generated member-menu module is 26624 bytes and is byte-identical
to the pinned retail module, SHA-256
`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`.
