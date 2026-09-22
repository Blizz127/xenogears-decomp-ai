#!/usr/bin/env python3
"""A/B the gears.toml SdataSmall preset change on its two removed TUs.

Variant A = SdataSmall (maspsx --dont-force-G0)   -- the old setting, which
            made build/out/slus_006.64 fail to link with truncated GPREL16.
Variant B = Default    (maspsx force-appends -G0) -- the setting committed in
            c1ea7b87.

Both variants are compared, function by function, against the retail
instruction words in asm/slus_006.64/**/*.s.  This is done at the .o level so
variant A can be measured even though it cannot be linked.  Because a .o has
unrelocated call targets, `jal` is blanket-masked here for BOTH variants
equally; the comparison between the two is unaffected.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-byte-parity-20260921/ab_sdatasmall.py
"""
import os
import re
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import analyze  # noqa: E402

TUS = ["system/temp1", "system/animation_scripts"]
SDATA_SMALL = "--dont-force-G0 --use-comm-section --run-assembler"
TMP = "/var/tmp/absdata"

OBJD_SYM = re.compile(r"^([0-9a-f]+) <([^>]+)>:")
OBJD_INS = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s")


def obj_funcs(path):
    txt = subprocess.run(["mips-linux-gnu-objdump", "-d", path],
                         capture_output=True, text=True, check=True).stdout
    funcs, cur = {}, None
    for line in txt.splitlines():
        m = OBJD_SYM.match(line)
        if m:
            cur = m.group(2)
            funcs[cur] = []
            continue
        m = OBJD_INS.match(line)
        if m and cur is not None:
            funcs[cur].append(int(m.group(2), 16))
    return {k: v for k, v in funcs.items() if v}


def cmp_masked(rw, bw):
    """(verdict, bad_words) with jal fully masked (unrelocated .o)."""
    if len(rw) != len(bw):
        return "size", None
    bad = 0
    for a, b in zip(rw, bw):
        if a == b:
            continue
        opa, opb = a >> 26, b >> 26
        if opa != opb:
            bad += 1
            continue
        if opa in (0x02, 0x03):
            continue
        if opa in analyze.IMM16_OPS and (a >> 16) == (b >> 16):
            continue
        bad += 1
    return ("exact" if bad == 0 else "codegen"), bad


def score(objpath, retail):
    fns = obj_funcs(objpath)
    ex = cg = sz = 0
    exact_bytes = retail_bytes = 0
    short = short_bytes = 0
    for name, bw in fns.items():
        if name not in retail:
            continue
        rw = retail[name][1]
        retail_bytes += len(rw) * 4
        v, _bad = cmp_masked(rw, bw)
        if v == "exact":
            ex += 1
            exact_bytes += len(rw) * 4
        elif v == "codegen":
            cg += 1
        else:
            sz += 1
            if len(bw) < len(rw):
                short += 1
                short_bytes += (len(rw) - len(bw)) * 4
    total = sum(len(v) for v in fns.values()) * 4
    return dict(exact=ex, codegen=cg, size=sz, short=short,
                exact_bytes=exact_bytes, retail_bytes=retail_bytes,
                short_bytes=short_bytes,
                text_bytes=total, comparable=ex + cg + sz)


def build_variant(label, sdata_small):
    shutil.copy("build.ninja", "/var/tmp/build.ninja.orig")
    if sdata_small:
        # NB: append is not enough.  animation_scripts already carries a
        # `maspsx_flags =` line from the XenoGraphics preset, and a later
        # binding in the same ninja block wins -- an appended line is silently
        # overridden.  Drop any existing binding in the block, then insert.
        lines = open("build.ninja").read().split("\n")
        out, i = [], 0
        while i < len(lines):
            l = lines[i]
            out.append(l)
            i += 1
            if any(re.match(rf"build build/src/slus_006\.64/{tu}\.c\.o: maspsx ", l)
                   for tu in TUS):
                while i < len(lines) and lines[i].startswith("  "):
                    if not lines[i].lstrip().startswith("maspsx_flags"):
                        out.append(lines[i])
                    i += 1
                out.append(f"  maspsx_flags = {SDATA_SMALL}")
        open("build.ninja", "w").write("\n".join(out))
    targets = [f"build/src/slus_006.64/{tu}.c.o" for tu in TUS]
    for t in targets:
        if os.path.exists(t):
            os.remove(t)
    subprocess.run(["ninja"] + targets, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    os.makedirs(f"{TMP}/{label}", exist_ok=True)
    for tu, t in zip(TUS, targets):
        shutil.copy(t, f"{TMP}/{label}/{tu.replace('/', '_')}.o")
    shutil.copy("/var/tmp/build.ninja.orig", "build.ninja")


def main():
    retail = analyze.load_retail()
    build_variant("A_sdatasmall", True)
    build_variant("B_default", False)

    print("== A/B of the SdataSmall preset removal, per TU, vs retail ==")
    print("A = SdataSmall (old, does not link)   B = Default (committed)")
    print()
    for tu in TUS:
        f = tu.replace("/", "_") + ".o"
        a = score(f"{TMP}/A_sdatasmall/{f}", retail)
        b = score(f"{TMP}/B_default/{f}", retail)
        print(f"{tu}")
        print(f"  {'':22}{'A (SdataSmall)':>16}{'B (Default)':>14}")
        for k in ("comparable", "exact", "exact_bytes", "codegen", "size",
                  "short", "short_bytes", "retail_bytes", "text_bytes"):
            print(f"  {k:22}{a[k]:>16}{b[k]:>14}")
        # raw byte divergence introduced by the switch
        pa = f"{TMP}/A_sdatasmall/{f}"
        pb = f"{TMP}/B_default/{f}"
        fa, fb = obj_funcs(pa), obj_funcs(pb)
        changed = [n for n in fa if n in fb and fa[n] != fb[n]]
        resized = [n for n in changed if len(fa[n]) != len(fb[n])]
        print(f"  functions whose bytes changed A->B: {len(changed)} "
              f"(of {len(fa)}), of which resized: {len(resized)}")
        print()


if __name__ == "__main__":
    main()
