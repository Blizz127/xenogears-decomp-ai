# Equip list native resource layout repair

The visible desktop run in scratchpad/lahan-natural-20260912-gear-desktop terminated with SIGSEGV in func_801DE5CC while reading equippedAccessory + 0xE. The read-only core showed accessories=NULL because retail resource offset +4 read the upper half of the native weapons pointer. The screenshot painting-ready.png also shows rendering defects; visual acceptance remains FAIL.

The list test previously supplied a retail-sized resource buffer to native code. It now supplies MenuUnk6 with widened native pointers, retaining the separate retail buffer for the MIPS oracle. The corrected test crashed before the production fix. Production now uses typed weapons/accessory pointers and retained 32-bit Gear slots inside unk8.

After repair: 352 differential cases at O0 and UBSan and four negative controls PASS. Existing O2 divergence remains unresolved. Native build LINK OK with 66 function stubs and 572 data symbols. These checks do not prove rendered Equip behavior.

Fresh rebuilt binary launched visibly on DISPLAY=:0 in scratchpad/lahan-natural-20260912-equip-layout-desktop; pins.json records binary hash and HEAD. Startup screenshot visible.png confirms a visible Squaresoft logo. Fresh Equip runtime acceptance is PENDING. No commit or push.
