# C1 retail RotMatrix, zero-Z specialization

Authority: `disc/SLUS_006.64`, SHA-256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`. Correct PS-X EXE mapping is file offset `0x800 + (VA - 0x80010000)`; retail `RotMatrix` `0x8003F738..0x8003F8B0` begins at file offset `0x2FF38`.

For C1's `vz=0`, name the signed Q12 table values `sx=sin(x)`, `cx=cos(x)`, `sy=sin(y)`, `cy=cos(y)`. Direct instruction derivation gives the row-major 3x3 halfwords:

```
tmp = (-sy * 4096) >> 12
m00 = cy
m01 = 0
m02 = sy
m10 = -((tmp * sx) >> 12)
m11 = cx
m12 = ((-cy * sx) >> 12)
m20 = (tmp * cx) >> 12
m21 = sx
m22 = (cy * cx) >> 12
```

Each multiplication is a signed MIPS `mult`, consumes LO, and each `>>12` is a separate arithmetic `sra`. Preserve these intermediate truncation points and 32-bit operations. In particular, retail `0x8003F870 mult t5,t7; 0x8003F880 mflo t0; 0x8003F890 neg t0,t0; 0x8003F898 sra t0,12` negates the complete `cy*sx` product before shifting for `m12`. Writing `-((cy*sx)>>12)` changes negative-product rounding and is incorrect. PsyCross `RotMatrix` instead constructs combined terms using `sin(z+x)`, `sin(z-x)`, `cos(z+x)`, and `cos(z-x)`, averages them, then multiplies. The algebra is real-number equivalent, but Q12 table rounding and intermediate truncation are not, matching the observed one-unit differences.

Within `animation_scripts.c`, the port shim intentionally maps the decomp names oppositely: semantic sine is `rcos(angle)` and semantic cosine is `rsin(angle)`. A local helper should assign explicit `sx/cx/sy/cy` variables accordingly. This repair should remain C1-specific; changing global PsyCross `RotMatrix` would affect unrelated renderer and gameplay users.
