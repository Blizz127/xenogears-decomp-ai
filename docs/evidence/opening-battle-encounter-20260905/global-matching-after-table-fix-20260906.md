# Private full matching check after text table fix

Date: 2026-09-06

## Scope

This is an isolated container run in `/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp`. The shared checkout was not built or modified. Before the run, the private copy was atomically replaced with current shared inputs:

- `gears.toml`: `0cec36c39ad07133f457cd7a92810e1d2412bf1bf947e2b7beec4b0bc0d88987`
- `src/slus_006.64/system/system.c`: `e1dbf43e5ecb9ca41c344e92e5109e92324ae3e4d7028c691e0575f3535f41bd`
- `src/slus_006.64/system/animation_scripts.c`: `8e5de18aa4c657cea9026626b4dd9a0d92f104788a4438024fa7a10c910bec5c`

The private configuration contains `-DINCLUDE_ASM_USE_MACRO_INC=1`. Relevant source/config pins:

- `include/labels.inc`: `9011fb521abe02bf51d16d3ae8ec1667ccd899a26affb6a5fe00b842ab421dc5`
- `include/macro.inc`: `a0887ca8e7a0befac2f08941dd379a3265e05354a2aee910b41d7184c933c6ca`
- `include/include_asm.h`: `b07191b5b52fe5a0cc20e2114349a7ccfb537bba948ba277f375d9de2559a16e`
- `linker/slus_006.64.ld`: `687ed0469386fdb55fd073e5817c0bb9afb6765ce7765203cc7a692fb0497269`
- `config/slus_006.64.yaml`: `5006574eec469695edc436a52b9e3dc8881241dd5319e72c94d753bd4771485f`
- `config/checksum.sha`: `a9fae6542c784778c9810c632da5a797e29a5c88f675ed79862daa2e1443e617`
- retail `disc/SLUS_006.64`: `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`

Container image: ID `94d6a0111bd27464273aa464784d43b08e7d3750ce371ef83bea6db9585e9f29`, digest `sha256:d7647c2346c43d7aa41d9a8da1c335296a57e178d97082ee37a391cbf345fc90`.

## Command and result

```text
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:rw \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai:ro \
  -w /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate; make check'
```

Result: exit `2`, log `container-current-make-check.log` SHA-256 `b76b6296cce8d070ee4fbf0fbaa01da8a6cbdd0010fc55ef08bf9339b8b11381`.

The build completed all `472/472` tasks, including every link and objcopy step. There were no compiler errors, undefined references, or `.L800...` jump-table diagnostics. The prior discarded-`.sdata` failure is also absent.

The final checksum gate remains red for four generated artifacts:

| artifact | actual SHA-256 | pinned retail SHA-256 |
|---|---|---|
| `build/out/slus_006.64` | `2eb8cb4bdba6aed54e1f0734bfb9c72d702ab5ee830139a58e70b432237cacca` | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| `build/out/field.bin` | `ac772e7e9e8feb5dd73856a5bf64980b4730f373ef90303eac021805551283d8` | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` |
| `build/out/member_change_menu.bin` | `242c5450a3faba50fee01d5924f21a64091e7dd27223de5c9c860925b361f2bc` | `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` |
| `build/out/shop_menu.bin` | `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` | `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` |

The log contains the established assembler `missing .end` warnings from `asm/menu/main/misc.s` (2,602 warning lines and 2,470 info lines); no `error:` or `undefined reference` lines occur.

Evidence: `/tmp/xeno-global-jtbl-audit-20260906/container-current-make-check.log`, `container-current-make-check.rc`, and this report.

## Header provenance

The private copy's `include/include_asm.h` is stale relative to the shared checkout. Private SHA-256 is `b07191b5b52fe5a0cc20e2114349a7ccfb537bba948ba277f375d9de2559a16e`; shared SHA-256 is `2d6d960c6002fcda260e3ead35a2707da29c2913f98f349d3285fd28522c40dd`. The only difference is the outer guard: shared adds `&& !defined(SKIP_ASM)`. The matching `gears.toml` flags contain no `SKIP_ASM` definition. Using the container's exact matching preprocessor command and separate live/private header overlays produced byte-identical output for both current `system.c` (SHA-256 `bf4f760f1a3abed9828a8b376b6092bedca3c4b6ada9a2fd19b151ebbce878ad`) and `animation_scripts.c` (SHA-256 `277c2aea83a61759a0bec58a9c11c1c941194849f692f256ec836285e4890bdd`). Therefore this run is not an identical-input claim, but the stale guard is matching-equivalent under the recorded flags; no rebuild was needed. The overlay probe is preserved under `probe/`.
