# Post-battle field/menu archive wait

Baseline 38ea3b7f. The user's live process returned from victory to field 15,
then stopped in func_800799D4 -> ArchiveCdDataSync(0) -> ArchiveDataSync.
D_800ADB64 was 0 (main menu), D_8004FDFC and drive state were both 1. The host
stream had four sectors remaining, no pending read, and eight available ring
slots after sector 80. HD2D drawing was absent from the stalled stack.

The host CD adapter advanced only through func_80028B14. A blocking archive wait
stopped the field consumer, so it could never finish those four sectors.

The port now separates transport from delivery. Blocking ArchiveCdDataSync
polls the native transport while retaining retail's status test. The producer
fills only free ring slots and assigns sector IDs; func_80028B14 delivers ready
sectors in order using the retail consumer cursor. Status-only ArchiveDataSync
remains side-effect free, avoiding an induced last-sector race with the field
consumer's completion/free test. No busy flags are cleared to bypass reads.

The regression originally failed with exit 10 (wait did not complete). The
production blocking-loop fixture now passes O0/O2/UBSan. Coverage includes
ready-sector preservation across the wait, delayed read completion, full-ring
backpressure, ordered delivery after slot reuse, and finished-ring lifetime.
Existing archive stream/seek structural checks pass. Native build and its
92-leaf link gate pass. Fresh MIPS builds preserve libarchive.c's whole .text
byte for byte; the only shared-source addition is under XENO_PC_PORT.

The user's game-state bytes, actor transform, map and entrance were read from
the stalled process and serialized using the production checkpoint writer.
The checkpoint payload equals the captured game-state bytes exactly. It is a
recovery snapshot, not a natural save-action acceptance test. The original
quicksave was not overwritten. Local evidence and recovered-victory.xgqs are in
scratchpad/astra-battle-return-20260919/.

A separate existing File-submenu failure was observed during testing:
func_801D9F98 is a generated stub; subsequent teardown attempts HeapFree(NULL)
and enters error 131. This patch does not implement that memory-card interface.
Use the host toolbar checkpoint controls rather than the File submenu.

Live validation: two natural encounters in the repaired build were won and
returned to the field with HD2D active; the main menu then rendered instead of
blocking in ArchiveCdDataSync. Selecting File exposed the separate stub noted
above. The final build was reopened on the desktop using the user's recovered
victory checkpoint; debugger observation confirms D_8004FDFC=0 and menu owner
D_800ADB64=255 (free field control). This does not certify the File submenu.
