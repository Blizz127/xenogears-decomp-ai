#!/usr/bin/env python3
"""Reference assertions for paletted PS1 semi-transparency.

This is intentionally a CPU oracle, not a renderer replacement.  It covers
the per-texel rule that PsyCross must implement: a CLUT bit-15-clear texel
overwrites B for every ABR mode; a set texel uses that mode's blend equation.
"""

BACKGROUND = (80, 120, 160)
FOREGROUND = (40, 60, 100)


def clamp(value):
    return max(0, min(255, value))


def abr(mode, background, foreground):
    if mode == 1:  # B + F
        return tuple(clamp(b + f) for b, f in zip(background, foreground))
    if mode == 2:  # B - F
        return tuple(clamp(b - f) for b, f in zip(background, foreground))
    if mode == 3:  # B + F / 4
        return tuple(clamp(b + f // 4) for b, f in zip(background, foreground))
    raise ValueError(mode)


def render(mode, clut_bit15_set, background, foreground):
    return abr(mode, background, foreground) if clut_bit15_set else foreground


for mode in (1, 2, 3):
    # bit 15 clear: PS1 writes F directly, irrespective of ABR.
    assert render(mode, False, BACKGROUND, FOREGROUND) == FOREGROUND
    # bit 15 set: PS1 selects the ABR equation.
    assert render(mode, True, BACKGROUND, FOREGROUND) == {
        1: (120, 180, 255),
        2: (40, 60, 60),
        3: (90, 135, 185),
    }[mode]

print("PASS: CLUT bit-15 clear overwrites; set entries use ABR 1/2/3")
