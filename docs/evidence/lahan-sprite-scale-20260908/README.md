# Sprite slow-motion scaling

Retail SLUS 80022CAC..80022CDC (48 bytes) multiplies the input by the unsigned 16-bit timer at sprite+0x3A, keeps LO, then applies signed rounding and arithmetic shift. The native owner used signed C multiplication. The pre-fix full production translation unit fails UBSan at timer2/input2147483647. This is an arithmetic boundary finding, not a claim that this exact value occurred in Lahan.

The native owner now multiplies unsigned 32-bit operands and reinterprets the retained low word for the existing signed rounding. No timing, floor, bounce, or script rules changed. The matching-tree owner in temp1.c remains unchanged and still needs its own exact-match and C-definedness audit.

The test executes the pinned original 12 MIPS instructions against the full native production translation unit. All 65536 timers crossed with14 signed arithmetic boundary inputs yield917504 cases each at O0/O2/UBSan, with all12instructions covered. It compares return values and256bytes of sprite/guards. Controls reject omitted negative rounding, omitted zero-timer passthrough, and the original signed product. This is not exhaustive over every32-bit input. Initial harness compilation required the established permissive flag for an unrelated existing Vsync declaration; that setup failure was not behavioral evidence.

Native build passed; see native-build.log. Full horizontal/vertical integration, retail audiovisual comparison, matching-tree byte identity, and start-to-end Lahan remain open. Earlier CompMatrix replay reached aftermathfield3 before a host reboot; this scaling edit was not in that frozen executable. No commit/push.

Follow-up: ../lahan-sprite-motion-20260908 now covers the native horizontal/vertical arithmetic against retail instructions with a controlled floor-query fixture. Real floor-query behavior, matching-tree identity, and scene parity remain open.

Matching-tree follow-up: ../lahan-sprite-matching-20260908 fixes temp1.c motion semantics and verifies byte identity for80022CAC(48bytes) and80022CDC(104bytes,3relocations). Vertical80022B2C remains396vs384bytes. Its bounded fixture passes, but full scene parity and native-override retirement remain open.

Ownership follow-up: ../lahan-motion-ownership-20260908 retires the three duplicate native motion owners after native-linked layout/arithmetic/mutant checks. The current build uses decompiled temp1.c; the already-running frozen replay predates retirement.
