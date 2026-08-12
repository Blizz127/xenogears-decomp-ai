# Fresh accepted regression matrix

The accepted matrix was enumerated from the current W34B5 validation set at execution time. There are **12 inherited logical rows**; all inherited rows passed unchanged. The new focused helper contributes three regime rows, for an exact **15-row total**.

| row | inherited acceptance | execution evidence |
|---|---:|---|
| `wm_800935DC` | 116/116 | `inherited_matrix_run.log` |
| `wm_80093660` | 208/208 | `inherited_matrix_run.log` |
| `wm_800923A8` | 72/72 | `inherited_matrix_run.log` |
| scheduler | 230/230 | `inherited_matrix_run.log` |
| P5 production + routing | 48/48 + 71/71 | `inherited_matrix_run.log`, `f_p5_routing.log` |
| P4 production + routing | 89/89 + 48/48 | `inherited_matrix_run.log`, `e_p4_routing.log` |
| P3 production + routing | 68/68 + 45/45 | `inherited_matrix_run.log`, `d_p3_routing.log` |
| P2 production + routing | 55/55 + 40/40 | `inherited_matrix_run.log`, `c_p2_routing.log` |
| P1 | 41/41 | `inherited_matrix_run.log` |
| P0 common tail | 22/22 | authority binary run; W34B5 validation |
| unaligned memory oracle | 43/43 | `inherited_matrix_run.log` |
| selector | 90/90 + exhaustive 65,535/65,535 | `inherited_matrix_run.log` |
| 73B04 O0 | 6/6 memory-state cases | focused certificate |
| 73B04 O2 | 6/6 memory-state cases | focused certificate |
| 73B04 UBSan O2 | 6/6, zero diagnostics | focused certificate |

No waiver was used. The inherited rows have no changed result. The focused helper rows are deliberately ahead of scheduler reachability and do not claim natural execution.
