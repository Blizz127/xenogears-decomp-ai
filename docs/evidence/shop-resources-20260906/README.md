# Shop resource loader — 2026-09-06

`ShopMenuLoadResources` now has a native C owner instead of a generated stub.
It resolves the packed archive, loads the menu resources, copies the retail
save-header/icon data, obtains the three portrait coordinates, uploads valid
party portraits, follows the debug sound-loading branch, retains shop entries,
and frees the same temporary resources in retail order.

The actual complete shop translation unit compiled with the project's
authoritative GCC 2.6 produces all 1,088 exact function bytes at
`801C54B4..801C58F4`, SHA-256
`f791af75ac8be21876feb8f706399780e3010ab17b780e8337fd328dcbfb2d51`.
Dependencies used by that function are pinned to retail addresses for the
isolated body comparison; unrelated externs have placeholder addresses.
This is not a whole-module exact-match claim. Earlier GCC 2.7 experiments
are historical only; the final evidence uses GCC 2.6.

The replacement also owns all 16 original `D_801C5000` bytes, including the
three bytes beyond the copied 13-byte filename. Root directly compared both
the function and data with the disc bytes, verified the production source
against the frozen candidate and its preceding source, and confirmed that
the final global shop binary's first 16 bytes match retail. The 68-byte lookup
repair is preserved. No linker or configuration change was made.

Root independently ran `pc_port/tests/run_shop_menu_resources_retail_test.sh`:
128 cases / 11,944 checks / all 272 retail instructions pass at O0, O2 and
all-Clang UBSan. Seven compiled controls fail: shortened filename, widened
packed pointer table, wrong portrait stride, missing coordinate adjustment,
skipping a valid character, reloading the captured archive global, and caching
the menu pointer across helper calls. Output:
`/tmp/xeno-shop-menu-resources.ZCWjcXlx`.

Fixtures compare boundary call order/arguments, copied bytes, complete menu
destinations and work tail, and the full portrait blob including untouched
bytes. Native menu/work/manager and returned resource/TIM buffers are above
4 GiB; packed resource slots remain 32-bit. Archive, decompression, heap,
atlas, GPU and sound boundaries are spies. These results do not prove actual
asset decompression, rendered shop UI, CD I/O or sound playback. The separate
archive destination pointer repair has its own actual-wrapper tests under
`../archive-buffer-pointer-20260906/`.

Private global matching completes all 472 tasks, retaining the same three
checksum failures. SLUS, field and exact member-menu hashes are unchanged.
Shop remains 55,296 bytes with 15 remaining inline assembly bodies and
unresolved function ordering; its SHA-256 is recorded in `matching-after.json`.

The native build links, removes this function from the generated stubs
(77 remaining function stubs), and has SHA-256
`7739b2b794a430eee899aade8be196e0c8b2d804cbce5d3bb451887b82872da6`.
It has not been played. The natural traversal keeps its copied preceding
`cda8623b` binary, which must not be restarted for this source checkpoint.
The forest route and later battles are still incomplete.

Scratch evidence: `/tmp/xeno-shop-resources-c-20260906`.
No licensed payloads, generated binaries, staging, commit or push.
