# Exact member-menu label drawer and initializer

Candidate: `astra-final.c`. Both functions compiled together using `compile-final.sh astra-final`, unchanged PSX GCC 2.6.0 optimization flags and MASPSX settings, with the existing fixed-VMA linker script extended only for the initializer symbols.

- `func_801C59E0`: 432/432 retail bytes exact, `0x801C59E0..0x801C5B90`.
- `func_801C5B90`: 92/92 retail bytes exact, `0x801C5B90..0x801C5BEC`.
- Combined: 524/524 bytes exact against disc/member_change_menu.bin `[0x9E0:0xBEC]`.
- Retail module SHA-256: `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`.
- Full hashes and comparisons: `astra-exact-check.json`.

## Source change and cause

The previous candidate generated index-pointer induction before item-pointer induction. GCC's loop RTL showed that the generalized induction-variable list is reduced from later uses first, with address variables combined into their latest matching candidate. Simply moving the item declaration past the first lookup was insufficient: a fresh `strIndices[i + 1]` expression after the item calculation caused the index pointer to remain first in that reduction order.

The exact source caches `u8* index = &strIndices[i]`, performs the first `GetStringEntry(..., *index)`, then sets `item = &base[i]`. The second lookup uses `index[1]`. This removes the later independent index-address calculation while retaining the retail helper's `&base[i + 1]` address path and stride temporary. GCC now initializes and increments the item pointer before the index pointer, matching both previously mismatched pairs.

Both lookups use explicit `entry` temporaries followed by fresh `g_Menu->unk4E0[0].pVramBuffer` loads. The initializer also allocates through that typed member and passes `g_Menu->unk4E0`. No raw host offset access remains in these candidates.

Diagnostic-only `-da` builds were used to inspect intermediate RTL; final exact evidence was rebuilt using the ordinary flags without `-da`. There is no assembly injection, binary modification, production edit, commit, or push. Native regression integration remains owned by root.

## Reproduce

```sh
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /tmp/xeno-member-labels-c-20260906:/tmp/xeno-member-labels-c-20260906:rw \
  -v /tmp/xeno-global-jtbl-audit-20260906:/tmp/xeno-global-jtbl-audit-20260906:ro \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai:ro \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate; bash /tmp/xeno-member-labels-c-20260906/compile-final.sh astra-final'
```
