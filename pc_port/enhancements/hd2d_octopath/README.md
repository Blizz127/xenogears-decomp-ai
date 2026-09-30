# Opening HD-2D enhancement

Owner task: `8773eca33b9e`. Branch: `codex/hd2d-octopath`, based on `claude/r6-pad` at `3d432128f6a71a5c1630c326c34a31ea3929ebd9`.

This branch prepares an independent desktop GL renderer backend, an exact-frame sprite atlas loader/compiler, and new Fei artwork. It does **not yet install a live menu entry or renderer hook**. The owner required hook agreement with the port integrator, and that agreement remains pending. Existing input, developer menu, transition fixes, legacy Fei settings, build source classification, and PsyCross files have not been edited.

The intended menu path is Options ▸ Enhancements ▸ **HD-2D (Octopath style)**. Master enablement is OFF by default, with independent effect toggles and intensities. The configuration also defaults to an empty opening-map allowlist. Integration must explicitly approve the prologue, Lahan and applicable interiors, and share one master flag between the renderer and new sprite replacement. Legacy rendering stays on its current path while this master flag is OFF.

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

The reviewable hook proposal and required attachment producers are described in `../../src/hd2d/integration-proposal.txt`. The inert patch in `../../patches/psycross_hd2d_present.patch.PROPOSAL` is not applied by the build. It puts processing after retail framebuffer storage and before screenshot recording, toolbar rendering and swap. Hash association and exact-frame fallback behavior are described in `SPRITE_MAPPING.md`.

New Fei art and its required attribution are in `assets/`. Only original project art is supplied. No area texture replacement, retail-derived upscaled texture, disc image or retail atlas is included. Areas will receive the look through the renderer, subject to hook integration and scene inputs. Additional Lahan cast artwork remains future work after the Fei direction and runtime mapping are validated.

Standalone checks:

```sh
bash pc_port/tests/run_hd2d_test.sh
bash pc_port/tests/run_hd2d_sprite_atlas_test.sh
python3 pc_port/tests/test_compile_hd2d_atlas.py
python3 pc_port/tests/test_describe_hd2d_cells.py
```

The Python PNG authoring tests require Pillow. Runtime C does not. Tests cover Off identity, exact-frame fallback, metadata validation, protected-pixel identity, true-depth behavior, fail-closed attachments and GL state restoration.

Private baseline builds, captures, guard evidence and review packages belong in `~/release-staging/xeno-work/hd2d/`. Opening before/after comparison and a Legion-class GPU performance run remain release requirements. Standalone success does not satisfy those in-game gates. Nothing is pushed or published.
