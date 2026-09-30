# Opening HD-2D enhancement

Owner task: `1a536996f2bf` (supersedes the earlier HD-2D assignments). Branch: `codex/hd2d-octopath`, rebased onto the integrator's hook and footprint commit `b84beffe0947066f2c199bf1704f8c3b28dfb083`.

The live post-process hook and enhancement registry are connected. The renderer and sprite transport compile through the lane's C-source build glob. The sprite transport preserves each original packet's screen vertices, colour, UVs, ordering-table slot and link order; a lane-owned PsyCross patch associates an original packet with its own nearest-filtered replacement texture during GPU consumption. Existing Fei, input, developer-menu and transition implementations are unchanged by this lane.

Verified production maps are field 14 (Fei's house) and field 1 (Lahan village). The profile maps the observed player actor1, FNV db7d0105, raw frame190 and its six exact parts to the original front-idle pose, with independently calibrated head crops for each scene. Every other identity/frame/map falls back to retail. The 16 authored poses remain intact. A private Lahan walking capture records 29 raw frames and 39 distinct part/UV variants, including different left/right variants of the same frame; these associations are not yet production mappings. Real GPU texture consumption is measured. The latest native house profile passed strict silhouette validation in four measured camera triples, with zero measured edge deltas and all 55 occluded rows matching. The earlier controlled field OFF check passes all 185 full-frame hashes and 24 screenshots using identical verified sprite render inputs in both binaries. Organic A/B gameplay captures remain phase-dependent as an NPC walks; their failed comparisons are retained. The fixture restores simulation state and forwards the original OT pointer.

The registered menu path is Options ▸ Enhancements ▸ **HD-2D (Octopath style)**. Master enablement is OFF by default, with independent effect toggles. Eight intensity rows register through the proposed weak numeric API when present: 0–100%, step 5. The menu owner must land that API and provide capacity for all 17 enhancement rows. Without it, the existing nine toggle rows remain available. Further integration must approve the prologue and other applicable interiors, and share one master flag between the renderer and new sprite replacement. Legacy rendering stays on its current path while this master flag is OFF.

| Effect | Controls | Required scene data |
| --- | --- | --- |
| Depth of field / tilt shift | Enable, intensity, focus and tilt | Verified linear depth or explicit OT-order proxy; sprite/UI protection |
| Bloom / glow | Enable, intensity | Sprite/UI protection |
| Warm lighting | Enable, intensity | Sprite/UI protection; projected lights; ambient works without depth |
| Sprite contact shadows | Enable, intensity | Projected feet and sprite/UI protection |
| Vignette | Enable, intensity | Current content viewport and sprite/UI protection |
| Fog | Enable, intensity | Verified linear depth or explicit OT-order proxy; sprite/UI protection |
| Particles | Enable, intensity | Projected particles, verified depth and protection |
| Colour grading | Enable, intensity | Current content viewport and sprite/UI protection |

Unavailable protection disables every composition effect. The backend supports explicitly distinct linear-camera-depth and normalized ordering-table modes; OT order is an ordering proxy, not physical camera depth. The latter uses R16 order and R8 coverage bits and disables physical-depth particles. See `ORDER_DEPTH.md` for units and protection semantics. The supplied tests demonstrate valid synthetic attachments; the current port still awaits the producer hook. Sprite textures use nearest sampling. Composition effects preserve protected sprite/UI pixels, and OFF bypasses GL operations entirely.

The landed hook ABI is in `../../include_shim/hd2d_sprite.h`; the exact runtime-part compiler and packet lifetime rules are described in `PART_MAPPING.md`. The older integration proposal is retained as design history. Scene input production and the HOOK4 adapter await the owner's landing commit. No depth or coverage texture is fabricated by this lane.

Original Fei art and candidate Alice art, with the required attribution, are in `assets/`. Alice's PNG and losslessly decoded runtime RGBA each pass the retail-byte guard with zero matches; her atlas has no live character/frame association yet. Native travel to her house stalls during field teardown, so cast mapping requires a verified checkpoint or capture from the owner. No area texture replacement, retail-derived upscaled texture, disc image or retail atlas is supplied. Areas receive the look through the renderer, subject to approved scene scope and verified scene inputs.

Standalone checks:

```sh
bash pc_port/tests/run_hd2d_test.sh
bash pc_port/tests/run_hd2d_sprite_atlas_test.sh
bash pc_port/tests/run_hd2d_bridge_test.sh
bash pc_port/tests/run_hd2d_sprite_parts_test.sh
bash pc_port/tests/run_hd2d_sprite_transport_test.sh
python3 pc_port/tests/test_compile_hd2d_atlas.py
python3 pc_port/tests/test_describe_hd2d_cells.py
```

The Python PNG authoring tests require Pillow. Runtime C does not. Tests cover Off identity, exact-frame fallback, metadata validation, protected-pixel identity, true-depth behavior, fail-closed attachments and GL state restoration.

`bash pc_port/tests/run_hd2d_benchmark.sh 1280 720 120` records the actual GL vendor/renderer and Off/all-effects median and 95th-percentile wall time, with warm-up, swap interval disabled and completion synchronized. Repeat at `1920 1080 120` on the approved hardware runner. It inherits the runner's GPU selection, uses a synthetic scene with depth/protection and sixteen lights/shadows/particles, and does not establish in-game frame rate. An integrated opening-scene performance comparison is still required.

Private baseline builds, captures, guard evidence and review packages belong in `~/release-staging/xeno-work/hd2d/`. Opening before/after comparison and a headless Alienware GPU performance run remain release requirements. Standalone success does not satisfy those in-game gates. Nothing is pushed or published.

The private Alienware synthetic benchmark ran on an NVIDIA RTX 5070 Ti Laptop GPU (driver 615.71.09): 120 samples after warm-up, all-effects median/p95 0.173554/0.178609 ms at 1280×720 and 0.420954/0.439134 ms at 1920×1080. These are composition timings, not in-game frame times or evidence of PsyCross batch capacity. A local native field run measured peak48 allocated PsyCross splits (4095 usable plus the sentinel), with zero capacity rejections. Integrated Alienware field timings and corresponding peak batch counts remain separate acceptance checks.

Final validation: full native build and strict Psy-Q exclusion pass. Title OFF identity pairs 428 render-input keys with zero mismatches (97% scene coverage). The opt-in lane path of `run_fei_hd2d_footprint_test.sh` passes strict tol1 with six comparable triples, 288 GPU acceptances and 1,112 changed Fei pixels. All ten pinned ROM artifacts match retail. Art payload guards report zero retail byte runs. Private captures and failed diagnostic runs stay in release-staging; test fixtures are excluded from the production source glob.
