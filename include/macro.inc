.macro glabel label
    .global \label
    .type \label, @function
    \label:
.endm

.macro dlabel label
    .global \label
    \label:
.endm

.macro jlabel label
    .global \label
    \label:
.endm

.macro nonmatching label, size
    /* splat-generated marker, no-op */
.endm

.macro enddlabel label
    .size \label, . - \label
.endm

.macro move a, b
    addu \a, \b, $zero
.endm

.include "gte.inc"
