# Retail matching baseline on PINNED splat — the flagged slus drift is a tooling artifact

Date: 2026-09-06 UTC. Observation/verification pass; no source change.
No commit/stage/push.

## Summary

The slus/field hash drift that `ACTIVE_HANDOFF.md` and
`docs/evidence/shop-string-uv-20260906/README.md` recorded as "FLAGGED, not
addressed" is **explained and, for slus, fully resolved**: it was caused by
building with a *different splat/spimdisasm version* than the pinned one, not by
any source regression. Building with the versions `requirements.txt` actually
pins reproduces the older recorded slus hash byte-for-byte.

## How this build was produced (evidence class: proven)

Both earlier failures to run the matching build in this checkout are now
resolved, and the two workarounds prior sessions relied on are unnecessary:

1. **The build container exists.** It is a *podman image*, not a distrobox
   container — `distrobox list` is empty, which is what led several checkpoints
   (and this session, earlier) to wrongly record "no container here / native
   build NOT_RUN". Use `localhost/xenogears-dev-toolchain:current`.
2. **No `mips-linux-gnu-cpp` wrapper is needed.** Prior sessions built a shim
   over the host `cpp` because the host lacks the MIPS preprocessor. The image
   ships the real `mips-linux-gnu-{cpp,as,objdump,...}`, so the wrapper (and its
   `-lang-c` workaround) can be dropped.
3. **No renamed tree copy is needed.** `tools/gears` hard-codes the project
   directory name `xenogears-decomp`; mounting this `xenogears-decomp-ai`
   checkout *at that path* satisfies it directly, so `make check` runs against
   the live working tree with no `cp -a`.
4. The image lacks the Python `splat` module; install it from `requirements.txt`
   into a venv (this is what pins the generator version).

```bash
podman run --rm --security-opt label=disable --userns=keep-id \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
  -v /var/home/blizz/.cache/xeno-venv:/venv \
  -w /home/blizz/Projects/xenogears-decomp \
  localhost/xenogears-dev-toolchain:current bash -lc '
    export TMPDIR=/var/tmp
    python3 -m venv /venv && /venv/bin/pip -q install -r requirements.txt
    export PATH=/venv/bin:$PATH
    make check'
```

`--userns=keep-id` is required (without it the mount is read-only to the
container user). `TMPDIR=/var/tmp` matters: `/tmp` here is a 12.3G-quota tmpfs,
and under quota exhaustion GCC 2.7.2's `cc1` has been observed emitting objects
with **no function bodies while still exiting 0** — a silent corruption mode that
can make byte-comparison results meaningless.

Generator actually used, printed by the venv before the build:
`splat 0.33.2 spimdisasm 1.33.0` — i.e. the versions pinned in
`requirements.txt` (`spimdisasm==1.33.0`, `splat64[mips]==0.33.2`,
`rabbitizer==1.12.6`).

## Result (evidence class: proven)

`make check` completes **472/472 tasks**, then exits 1 on the retail checksum
gate with the same three known-red artifacts:

| artifact | size | SHA-256 (first 16) | gate |
| --- | --- | --- | --- |
| `build/out/slus_006.64` | 301,636 | `5c674b3f73c7ffce` | FAILED |
| `build/out/field.bin` | 242,622 | `6b7f53a8263545f1` | FAILED |
| `build/out/member_change_menu.bin` | 26,624 | `3b9e2b890c27ae0d` | **OK** |
| `build/out/shop_menu.bin` | 55,296 | `770921ded3bb1232` | FAILED |
| `build/out/menu.bin` | 152,860 | `9fc9b811b273089f` | not gated |

### Interpretation against the prior records

| artifact | 02:37 record (`shop-resources`) | 10:55/12:16 record (host splat 0.41.1 / 1.42.2) | this build (pinned 0.33.2 / 1.33.0) |
| --- | --- | --- | --- |
| slus_006.64 | `5c674b3f…` | `3192a514…` | **`5c674b3f…`** — reproduces the 02:37 record exactly |
| field.bin | `f72b2245…` | `ececa463…` | `6b7f53a8…` — matches neither |

- **slus: proven tooling artifact.** Pinning the generator reproduces the older
  hash exactly. Nothing in slus source regressed. The earlier conclusion that
  the drift came from "other uncommitted field/slus edits already in the tree"
  was wrong for slus.
- **field: expected to differ from both.** `field.bin` legitimately changed
  during this session — four functions were landed byte-exact and one as a
  coexistence body in `misc2.c`/`misc5.c`/`misc9.c`, plus a corrected call site
  (see `docs/evidence/field-asm-only-20260906/`). So its hash *should* be new.
  This build is the first field baseline that is both post-those-changes and on
  pinned tooling; treat `6b7f53a8263545f1` @ 242,622 bytes as the current
  reference and state the generator version alongside any future record.
- `member_change_menu.bin` still matches retail exactly; `shop_menu.bin` and
  `menu.bin` reproduce their shop-string-uv checkpoint values.

**Consequence for the project:** every recorded slus/field hash is a function of
the splat/spimdisasm version as well as the source. Baselines that do not state
the generator version are not comparable. Builds should use the pinned versions.

## Corrections to other records in this session

- **The field overlay DOES link.** `docs/evidence/field-asm-only-20260906/`
  reports that the field overlay "does not currently link in this tree" because
  `src/field/main/misc6.c` leaves `func_800A0DC0`/`func_800A0E54`/
  `func_800A0EB0`/`func_800A0EE8` and others undefined. This build links
  `build/out/field.elf` and produces `field.bin` at 472/472 tasks, so that claim
  does not hold for the live tree — most likely an artifact of that pass's own
  quota-broken scratch environment (see the `cc1` silent-empty-object mode
  above), or of a mid-edit snapshot of `misc6.c`.
- The same evidence dir records "no runtime observation… the container doesn't
  exist here". It does; see the invocation above and
  `docs/evidence/blackmoon-map16-runtime-20260906/`.

## Not proven

- field.bin is still **18,240 bytes short** of retail `disc/field.bin`
  (260,862 bytes, `38a1ce82…`), so it cannot match yet regardless of tooling.
- Whether the *remaining* slus/shop checksum failures have any tooling component
  was not investigated; only the slus record-vs-record drift was resolved.
- The specific `.bss`/`.sbss` symbol whose size/alignment differs between the two
  generators is still unnamed; with slus now reproducing on pinned tooling this
  is documentation, not a blocker.
