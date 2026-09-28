# Port vs retail divergences

One line per known place where the port's C (or data) behaves differently
from the retail code. Append-only, grouped by overlay; within an overlay keep
lines sorted by function. Format: `func (file): what retail does vs what the
port does [status]`.

## field

- func_800A708C (src/field/main/misc5.c): retail passes D_800C3A36 zero-extended (lhu) to func_801D3538; the port sign-extended it [fixed in 063a970a]
- func_800ACB90 (src/field/main/misc9.c): retail calls FieldLoadTIMWithClut(D_800AF784, 0x380, 0x100, 0, 0x1FF, 0, 0); the port's C passes y = 0 [open; function still INCLUDE_ASM]
