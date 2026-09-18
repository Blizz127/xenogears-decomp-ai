# Lahan/Equip continuation, 2026-09-13

Priority: resume recorded Lahan/Equip issues. No commit or push.

## Equip oracle repair

The O2 divergence reproduced at seed=1, category=0, gearMode=1, group=0:
host return 55, oracle return 0. An O2 owner with O0 harness passed; an O0
owner with O2 harness failed. Read traces first diverged after the bridged
mask helper. The oracle had read character 0 rather than 1, then Gear ID
0x9B from the patterned state, outside the host mask table.

Root cause: mapping SystemMenu by sizeof(native menu)=0x20C8 covered the
next guest allocation at 0x80102000. The manager read at 0x80102031 thus
read menu storage. Separate retailMenu[0x1E98] restores the actual retail
allocation boundary. MenuManager itself retains its 0x30 character-ID offset.
The test now checks character/Gear bus mapping before every retail execution.
It also executes the actual retail mask helpers (801C865C/801C8678) instead
of bridging both sides through host implementations.

Validation: run_menu_equip_list_test.sh with LINK_CC=gcc in
localhost/xenogears-dev-toolchain:current. 352 cases each at O0, O2 and
O1+UBSan pass. Four optimized behavioral mutants and the former overlapping
guest mapping are rejected. The oracle script validates 2824 instruction
bytes against disc/menu.bin, SHA256
82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d.

Scope remains list output/return behavior with renderer fixtures; this is not
visual Equip acceptance. Gear-accessory cases and the existing title-render
exclusion remain explicit test gaps. No production Equip behavior changed.

## Retail build progress

Moved the declaration of h before statements in func_8007D478 (main27.c) so
the retail compiler accepts it. Its differential harness now uses the common
oracle location resolver: 192 cases at O0/O2/UBSan and mutant rejection pass.
No byte-exact C claim. In an isolated snapshot, full battle compilation now
reaches linking. It fails with 73 missing local jump-table label references
(e.g. .L80079F00), from existing C transcriptions and retained retail rodata.
Do not remove those relocations or substitute addresses to force a link.

## Native build and runtime status

Maintained native build: LINK OK, 66 function stubs and 572 data symbols.
Binary SHA256 b7229f5ef4c72bf55a5d3b6980666f6efec94d99dab6724675c066afa952813b
is unchanged from the recorded desktop build, consistent with harness-only
Equip changes and reference-only battle code.

The recorded desktop run scratchpad/lahan-natural-20260912-equip-layout-desktop
ended rc=-6 in stage=newgame; its last log line is stub func_801D9F98.
The driver had sent Up/Circle at the title but never logged New Game confirmed.
Do not treat this as having reached Lahan, or infer the correct menu selection
from the driver's intended input. Fresh natural Equip visual validation,
ghost text and missing stats remain unresolved. Next: observe title selection
and actual runtime path, then reach Equip through ordinary controls.
