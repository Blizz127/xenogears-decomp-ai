# Checkpoint collision restore and unsupported File menu

The latest closed run ended in `func_801D9F98`, the native File-menu stub.
Entering this unfinished screen permits cleanup of objects never allocated.
The main menu now rejects that entry before navigation teardown and presents
an explicit unavailable message. This does not implement memory-card saving
or certify title-screen Continue. Toolbar field checkpoints remain available.
Live testing opened and dismissed the message, then cancelled back to field.

A separate movement stall was reproduced after loading the recovered victory
checkpoint: Fei turned with directional inputs but remained at
(696, -18, -803). Read-only inspection found walkmeshId=0, a non-null
D_800AFB24[0]=0x75ffd4, and cached triangle 60. Its vertices were
(578,-1728), (578,-1584), (747,-1656), outside the saved position.
The containing triangle was 845. Checkpoint restoration had changed position
without updating the walkmesh triangle cache created at map entry.

The port checkpoint loader now repeats the triangle/material/normal lookup
used by retail actor placement (`func_8009E574`), preserving the saved fixed
point coordinates and height. Retail placement and movement logic are unchanged.

## Verification

- Native build and 92-leaf generated-stub reachability gate passed.
- File guard and checkpoint restore regressions fail against their respective
  pre-fix sources; repaired sources pass O0, O2 and undefined-behavior sanitizer.
- Checkpoint file/request and archive transport regressions passed.
- Fresh MIPS compilation of the entire menu translation unit before/after has
  identical `.text`: SHA256
  `7ff8ccc25bf6ba12b25cd80e2e813799fb77cbf9c2a5271a32ced06230ee2504`.
  This proves unchanged MIPS output for this patch, not full retail parity.
  Checkpoint restoration is a native-only translation unit.
- Live rebuilt game loaded the unchanged recovered victory checkpoint.
  Left input moved from (696,-18,-803) to (591,-35,-700).
  Opening/closing the main menu returned to field; subsequent Right input
  moved to (674,-22,-784). HD-2D remained enabled.
- No new battle was completed for this patch; previous battle-return transport
  validation is documented separately. Broader gameplay parity is unverified.

Raw debugger captures, mesh dump, red/green runs, builds, MIPS comparison and
runtime logs are local in `scratchpad/astra-file-menu-20260919/`.
User saves and recovered victory snapshot were not overwritten.
