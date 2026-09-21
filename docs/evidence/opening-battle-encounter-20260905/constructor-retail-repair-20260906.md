# Retail constructor repair and opening replay, 2026-09-06

The installed native dialogue constructor now retains raster allocation bytes,
as retail does, and reproduces retail signed-halfword truncation and packed
coordinate/width arithmetic without signed-shift overflow. The established
eight-argument native ABI and target-specific guest bridge remain intact.

Source: src/slus_006.64/system/system.c. Before SHA256
6e9b2c746304d38cdb72de0cb22f112a6c2809e90cbbd42ef02e43297a66bdc8;
after b75fb09b069abd5462b73c0530b7365879cf2f5af5dfa8256a05bd807a4a67db.
Root reviewed the bounded diff and independently reproduced production RED,
candidate GREEN, seven explicit semantic failures and the old UB diagnostics
in /tmp/xeno-window-retail-repair-20260906/root-differential.

Installed production regression:
- pc_port/tests/run_battle_window_constructor_retail_test.py: 124 cases/mode,
  112 complete constructors plus12 bounded allocator stops; O0/O2/ClangUBSan
  PASS and all7 controls rejected. Retail constructor/GPU helper instructions
  execute directly; heap allocation policy is an explicit nonclearing stub.
  Root retained output: /tmp/xeno-window-constructor-retail-h3chfeyu.
- pc_port/tests/run_glyph_raster_retail_test.sh: 144 synthetic glyph cases/mode
  against raw retail glyph instructions, both interleaved pages,13 full rows,
  retained patterns and negative glyph offsets; O0/O2/ClangUBSan PASS, both
  controls rejected. Root retained output: /tmp/xeno-glyph-raster-retail.kpFpRS.

Native build LINK OK, binary
ed9987ed2a14639acca2edeab0a2ad25cc04ddef7a44aebf7e8d7eef107c1420.
Matching before/after both fail at resident linking with the same explicitly
listed undefined-label reference multiset. There are72 diagnostic lines,
including69 individual references and3 summaries; total references are not
known from that count. No matching pass is claimed. Logs and install proof
are under /tmp/xeno-window-retail-repair-20260906.

The first virtual replay ysi05oci displayed Hiyaaaaa and, after one normal z
press, Huff,huff; it reached its owned300-second limit before further input.
This was a test-driver timeout, not a game input defect. The second replay
jwlhtzig used fresh ordinary z press/release pulses during battle dialogue.
It visibly rendered Fei panels, completed the scripted battle, and returned
directly to the painting room. Sparse read-only probes recorded result1,
map14, computed state1, and actual FieldMain entry at04:05:48UTC.
g_CurGameState0 at FieldMain entry is the normal MainLoop clear-before-call
behavior; no unexpected KernelMenu breakpoint fired.

The replay used an isolated Xvfb display and null audio output, normal NewGame
keys at1x, then the existing F11 speed control to5x. The field-only confirm
schedule remained test tooling; no story/position/RAM/register state was
planted. The monitor requested termination eight seconds after field return,
preserving frames and trace. It stopped its own native/debugger/Xvfb processes.
No operator desktop window or prior recording was changed.

See constructor-after-runtime-20260906.json for source, binary, test-output,
input, trace and image pins. Runtime proves this native opening on the virtual
display. Full retail framebuffer/audio parity and complete decompilation remain
separate open gates.
