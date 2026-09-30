# Opening HD-2D enhancement

Owner task: `1a536996f2bf` (supersedes the earlier HD-2D assignments). Branch: `codex/hd2d-octopath`, rebased onto the integrator's hook and footprint commit `b84beffe0947066f2c199bf1704f8c3b28dfb083`.

The live post-process hook and enhancement registry are connected. The renderer and sprite transport compile through the lane's C-source build glob. The sprite transport preserves each original packet's screen vertices, colour, UVs, ordering-table slot and link order; a lane-owned PsyCross patch associates an original packet with its own nearest-filtered replacement texture during GPU consumption. Existing Fei, input, developer-menu and transition implementations are unchanged by this lane.

The only approved production map is field 14, verified by the integrator's Fei-house checkpoint. Exact animation/part profiles remain empty pending verified port traces. This prevents guessed scene or animation associations. The supplied 16-pose Fei artwork is an authored review asset; it is not yet a verified live replacement. The strict horizontal silhouette gate and field-scene ON replacement check remain required before claiming that integration complete.

The registered menu path is Options ▸ Enhancements ▸ **HD-2D (Octopath style)**. Master enablement is OFF by default, with independent effect toggles. Per-effect intensity setters exist; visible numeric menu controls still require agreement with the menu owner. The bridge defaults to the verified field-14 interior. Further integration must explicitly approve the prologue, Lahan and other applicable interiors, and share one master flag between the renderer and new sprite replacement. Legacy rendering stays on its current path while this master flag is OFF.

| Effect | Controls | Required scene data |
| --- | --- | --- |
| Depth of field / tilt shift | Enable, intensity, focus distance and tilt | Verified linear depth and sprite/UI protection |
| Bloom / glow | Enable, intensity | Sprite/UI protection |
| Warm lighting | Enable, intensity | Projected light positions; ambient works without depth |
| Sprite contact shadows | Enable, intensity | Projected feet and sprite/UI protection |
| Vignette | Enable, intensity | Current content viewport |
| Fog | Enable, intensity | Verified linear depth and sprite/UI protection |
| Particles | Enable, intensity | Projected particles, verified depth and protection |
| Colour grading | Enable, intensity | Current content viewport |

Unavailable depth or protection inputs disable dependent effects. Screen position and ordering-table buckets are not substitutes for camera depth. The supplied tests demonstrate the backend with valid synthetic attachments; the current port does not yet produce the required attachments. Sprite textures must use nearest sampling. Composition effects preserve protected sprite/UI pixels, and Off bypasses GL operations entirely.

The landed hook ABI is in `../../include_shim/hd2d_sprite.h`; the exact runtime-part compiler and packet lifetime rules are described in `PART_MAPPING.md`. The older integration proposal is retained as design history. It is not the current hook contract. Scene input production is still pending: no depth or protection texture is inferred from screen coordinates or OT slots.

New Fei art and its required attribution are in `assets/`. Only original project art is supplied. No area texture replacement, retail-derived upscaled texture, disc image or retail atlas is included. Areas will receive the look through the renderer, subject to approved scene scope and verified scene inputs. Additional Lahan cast artwork remains future work after the Fei direction and runtime mapping are validated.

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

The private Alienware synthetic benchmark ran on an NVIDIA RTX 5070 Ti Laptop GPU (driver 615.71.09): 120 samples after warm-up, all-effects median/p95 0.173554/0.178609 ms at 1280×720 and 0.420954/0.439134 ms at 1920×1080. These are composition timings, not in-game frame times or evidence of PsyCross batch capacity. Integrated field timings and peak batch counts remain separate acceptance checks.
