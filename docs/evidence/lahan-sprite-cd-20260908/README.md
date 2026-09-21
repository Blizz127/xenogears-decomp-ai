# Sprite opcode CD: retail-backed angle update

The frozen run `scratchpad/lahan-natural-20260908-93` aborted at 00:49:32 CDT in opcode CD during its first mountain encounter after normal Escape input. The core records sprite 0x74AC28, operands 0x73039B (`05 00`), model 0x74ACDC, model angle initially zero. Field return was not observed. No debugger was attached when the assertion occurred.

Retail SLUS dispatcher table 0x800183D8 maps CD to 0x8001FF0C. Instructions through 0x8001FFBC select model+0 or direction[index]+2 using index bits 9..11; the low nine bits give an angle multiplied by eight. Bit12 selects assignment versus addition modulo 16 bits. Null model or missing indexed direction buffer returns without mutation. Only index zero reaches 0x800215A4 and sets sprite+3C bit0x10000000, after the angle write.

The test executes pinned retail instructions through the MIPS adapter and compares the entire fixture with the production C dispatcher. It covers all 65,536 operand values across null-model, missing-direction, and valid states, plus model/sprite and operand/data overlap. Dependency guards reject unexpected native helper calls. This is behavioral evidence, not compiled-byte or rendered parity. The runtime repair remains pending a new normal boot.

Run `bash pc_port/tests/run_sprite_dispatch_cd_retail_test.sh`. See red.log, green.log, native-build.log, psx-build.log, and crash-state.log. Test-generated files and licensed payloads stay outside Git.

Subsequent normal run `scratchpad/lahan-natural-20260908-cd` observed two combat victories and repeated field15 returns, with selector83/03 verified after its first two returns. It remains live while a ledge-movement restriction is diagnosed. See the run CURRENT.md. These observations do not prove opcode-specific execution or full Lahan parity.
