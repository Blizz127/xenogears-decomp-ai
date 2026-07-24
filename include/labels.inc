# This file is used by the original compiler/assembler.
# Defines the expected assembly macros.

.include "gte_macros.inc"

# A function symbol.
.macro glabel label, visibility=global
    .\visibility \label
    .type \label, @function
    \label:
        .ent \label
.endm

# The end of a function symbol.
.macro endlabel label
    .size \label, . - \label
    .end \label
.endm

# An alternative entry to a function.
.macro alabel label, visibility=global
    .\visibility \label
    .type \label, @function
    \label:
        .aent \label
.endm

# A label referenced by an error handler table.
.macro ehlabel label, visibility=global
    .\visibility \label
    \label:
.endm


# A label referenced by a jumptable.
# Visibility must be GLOBAL: this project migrates jump-table rodata into
# separate data .s objects (e.g. slus data/800.rodata.s), whose `.word .LXXX`
# entries can only resolve against exported labels.  include/macro.inc (the
# raw-asm path) has always globalized jlabel; a `local` default here leaves
# every INCLUDE_ASM'd function's jtbl targets unexported and breaks the slus
# and field links (jtbl_80018664 -> .L80023490 et al).
.macro jlabel label, visibility=global
    .\visibility \label
    \label:
.endm


# A data symbol.
.macro dlabel label, visibility=global
    .\visibility \label
    .type \label, @object
    \label:
.endm

# End of a data symbol.
.macro enddlabel label
    .size \label, . - \label
.endm


# Label to signal the symbol haven't been matched yet.
.macro nonmatching label, size=1
    .global \label\().NON_MATCHING
    .type \label\().NON_MATCHING, @object
    .size \label\().NON_MATCHING, \size
    \label\().NON_MATCHING:
.endm
