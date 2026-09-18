# W34N127 — retail state-0/1 slice of `wm_8008E76C`

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`
- Date: 2026-09-03
- Retail state slice: `[0x8008EB30,0x8008EB64)`
- Retail shared tail: `[0x80090620,0x800906B4)`
- Slice SHA-256: `0f0e3b8819a5c572a771ca2ae6c31117586904d6af7261163e0ef20699627db0`
- Tail SHA-256: `d1e69ec3a0803d27cac240edc37e22659741c30b8ccfb2d7c38606d8f1adde5c`
- Verdict: **RETAIL_SLICE_AND_PARENT_ROUTE_VERIFIED**

## Closure

The parent callback now follows retail's `slot+0x04 != 4` branch directly to
the signed `slot+0x20` state dispatch. States 0 and 1 execute the bounded
`0x8008EB30` body: sample terrain, store `terrainY - 0x1000` before the ring
helper, then enter the shared `0x80090620` tail. State 2 uses that same tail.
The legacy lap-match continuation remains outside this bounded slice.

## Certificate

`pc_port/tests/run_w34n127_vehicle_state01.sh` compiles the production parent
and drives states 0, 1 and 2 through it. Adversarial `slot+0x04` values
`1, 5, 6, 7, 8, -1` prove that the former invented pre-dispatch arms cannot
intercept the retail state switch. The ring-helper mock snapshots the vector
at call time, proving the Y store precedes the call.

O0, O2 and nonrecovering UBSan pass. The earlier W34N123 state-2 certificate
also passes O0/O2/UBSan and detects mutants M1-M10.

## Natural-route regression

The post-correction W34N124 Lahan run passed:

```text
position 29947,-312,11075 -> 30116,-309,10905
walk animation 1; rendered pose 14 -> 24
trigger record id 1
[worldmap-open-loop] natural state exit ... D7CC=0
[FieldMain] ... g_GameSceneMapNum=1
no [worldmap-stub]
```

Runtime log: `/tmp/w34n124-lahan.NYJmZY.log` (ephemeral local evidence).
