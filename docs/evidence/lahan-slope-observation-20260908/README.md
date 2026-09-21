# Lahan mountain steep-edge observation

Live frozen CD run1019197 remains running. At X288,Z976, layer0 triangle376, material0x400000, ordinary movement stopped advancing after a Square jump and subsequent battle return. Input masks reach the field update, the control gate is zero, and the actor has no active jump0x800 flag. Sprite gravity+1C is0x20000. No game state was directly changed.

Short temporary breakpoints, removed on hit and detached in under a second, captured the slope and collision chain. The slope raises actor+F0 from0x10000 to0x30000, then eventually gives nonzero motion. Native func8007BAC0 receives angle1243 and vectorFFFFAB70,0,FFFF0E00. Its first probe succeeds; second probe rejects; the edge-projected final vectorFFFFE163,0,FFFEF066 rejects at material0x800000. That resets accumulation.

Offline retail walker replay matches the captured final rejection and edge coordinates. Full resolver replay also matches the three probe vectors and final failure when NCLIP uses integer screen coordinates. An initial scratch replay using the PGXP-enabled PsyCross NCLIP path incorrectly returned success because CPU register writes did not update the enhanced floating coordinates. That was a harness error, corrected before any collision repair. No production collision or jump change was made.

The current resolver failure is retail-consistent for the captured state. This does not prove the earlier native jump/field restoration correctly produced that state, nor that the complete runtime matches retail. Further route and entry-state investigation is required. A normal menu open/close succeeded but did not establish recovery; another encounter occurred afterward.

Scratch inputs remain in the live run directory: walker-actor.bin, materials.bin, and prior field15 triangles/vertices under lahan-natural-20260907-arithmetic/field15-data. The replay loads the pinned SLUS and field overlay. The full resolver uses PsyCross for other GTE operations and explicit integer NCLIP; it is a diagnostic replay, not an exhaustive production test or audiovisual proof.
