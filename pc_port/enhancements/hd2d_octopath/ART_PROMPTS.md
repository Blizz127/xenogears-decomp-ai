# Fei art provenance

fan-made, AI-generated replacement sprite art created for this project; not from the game; characters © Square Enix

Created 2026-09-30 with the session's built-in `image_gen.imagegen` tool. No API key, retail extraction, texture upscaler, or external asset download was used. The identity reference was the existing project-made AI Fei atlas, rather than a retail sprite sheet. Only the new output is included here.

The initial art direction requested new hard-edged pixel art for a compact RPG character, inspired by the owner's Octopath HD-2D reference: brown tousled hair and ponytail, cream and navy martial shirt, navy wrist wraps, teal sash and trousers, and brown shoes. It requested a transparent four-by-four sheet, rows front/back/left/right, with idle/left-step/passing/right-step poses; consistent foot baselines; small discrete color clusters; and no painterly smoothing, gradients, weapons, text, logos, or ground shadows. This paragraph records the art direction rather than claiming to reproduce the entire initial prompt verbatim.

An initial correction requested consistent directions and crisper clusters. Two left-facing poses still faced right. A final image-tool correction changed only the third row's third and fourth poses to face left, preserving the other poses and transparent background. The final tool output was copied without resampling or other creative image editing.

The tool returned a 1254 × 1254 RGBA image rather than the requested 1024 × 1024. Its silhouettes cross some equal-grid boundaries. `describe_hd2d_cells.py --content-bounds` therefore measures opaque connected components, retains complete silhouettes with a one-pixel margin, and produces the adjacent authored-cell manifest. This is read-only image inspection, not art generation. PNG-to-RGBA conversion for private guard checks preserves pixels exactly.

The sixteen authored poses are an art-review asset. Their names describe the requested poses, not verified retail animation frames. Runtime frame indices, foot anchors, gait consistency, and camera-relative facing still need in-game approval. No production animation mappings or retail hashes have been invented.

The binary guard is run on ELF wrappers containing the unchanged PNG and decoded RGBA because the existing guard expects an ELF input. Private logs and JSON results are kept under `~/release-staging/xeno-work/hd2d/art-guard/`. These are art-specific checks, not evidence that the unintegrated renderer has passed the complete port release gates.
