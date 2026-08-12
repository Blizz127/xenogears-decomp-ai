# 0x80073B04 packet-state oracle

The certificate snapshots and compares, per fixture:

- all 160 bytes at `0x8009C744` (`POLY_FT4[4]`, 40 bytes each);
- all 24 bytes at `0x8009D3D8` and `0x8009D3E4` (both 12-byte DR_TWIN carriers);
- the complete touched `OT[bucket]` word;
- projected `xy0..xy3` in both active packet slots, last-quad OTZ, and last-quad flag;
- the publication trace for the equal-postimage OT-link mutant.

The oracle independently seeds the W21B template image, the static retail vertices, the DB→OT pointer, camera matrix, theta, double-buffer index, runtime OT shift, and a distinct OT preimage. It then independently performs the accepted `RotMatrixYXZ → clear R.t → CompMatrix(camera,R,C) → SetRotMatrix → SetTransMatrix → RotTransPers4×2` sequence into local expected state and derives the link words with explicit `0xFF000000`/`0x00FFFFFF` masks.

Retail insertion order is mechanically certified as:

1. `twinB` tag receives old OT low 24 bits; OT receives `twinB` low 24 bits.
2. `prim0` tag receives the forwarded `twinB` OT word; OT receives `prim0` low 24 bits.
3. `prim1` tag receives the forwarded `prim0` OT word; OT receives `prim1` low 24 bits.
4. `twinA` tag receives the forwarded `prim1` OT word; OT receives `twinA` low 24 bits.

The resulting head-first draw chain is `twinA → prim1 → prim0 → twinB → prior`. Tag high bytes remain `0x02` for DR_TWIN and `0x09` for FT4; the OT high byte remains its pre-existing value.

The sweep uses theta `0x0000`, `0x0400`, `0xF800`, and `0xFF00`; indices 0/1; runtime shifts 2/3; and camera depths 8192/12288 with asymmetric camera rotation/translation. This covers the S1–S8 hazards:

- S1: both indices are checked across all 160 packet bytes;
- S2: zero is paired with nonzero quarter/negative/near-wrap headings;
- S3: packet-specific XY is checked, not only the union silhouette;
- S4: bucket word is checked directly under multiple depths/shifts;
- S5: the production path uses explicit arithmetic shift semantics and the source mutant is exercised;
- S6: tags are checked before any reinitialization;
- S7: nonzero UV scroll checks the DR_TWIN bracket;
- S8: this submitter is certified with the sibling submitter absent, so no second-submitter masking occurs.
