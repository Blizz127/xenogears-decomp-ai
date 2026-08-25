# W34B72 world capture boundary

The world-loop screenshot helper previously called `glReadPixels` after
`wm_800712D0` returned.  A same-run discriminator showed malformed terrain at
the effective `PsyX_EndScene` presentation boundary while that later read
returned the dark orange-text scene.

Frames 58 through 62 each called `PsyX_EndScene` four times: three calls found
the scene closed and returned without presenting; one call found it open and
performed the frame's only swap.  The three closed call sites remain
unidentified.  They are harmless no-ops now, but a future scene-lifetime change
could turn one into an unintended second present.

W34B72 changes capture into a request made by the world loop and fulfilled by
that sole effective `PsyX_EndScene` call before the swap.  The request records
its world-frame number and path; fulfillment requires the same current frame,
clears the request, and records the fulfilled frame.  Overwrite, frame mismatch,
and bounded exit with a pending request are errors.

Native 120-frame acceptance:

- frame 60 request/fulfillment: `60 == 60`
- frame 120 request/fulfillment: `120 == 120`
- frame 60 SHA-256: `493d8aec7b4d814b376de3bb0995e9d925edc3cf571702894a381845384bb381`
- frame 120 SHA-256: `be873d0437c7d7008273072475ba5c2720246dfc355f2161cd7692c3359b81e8`
- frame 60 exactly matches the earlier presentation-boundary oracle
- both captures contain the independently tracked malformed world geometry

Vendor-side diagnostic build note:

```sh
cmake -S pc_port -B pc_port/build -DCMAKE_C_FLAGS=-DXENO_DIAG_NAME -DCMAKE_CXX_FLAGS=-DXENO_DIAG_NAME
XENO_DIAG_DEFINES=-DXENO_DIAG_NAME ./pc_port/build_port.sh
```

Both CMake cache flags must be restored to empty before the final normal build.
