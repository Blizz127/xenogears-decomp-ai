# Native stub classification guard

Observed failure: after a full PSX build failed and only menu.elf was rebuilt, gen_port_stubs.py treated586 unclassified symbols as functions, including data, and the native link succeeded. A full overlay rebuild restored correct classification but did not prevent recurrence.

Repair: the generator rejects every undefined symbol absent from supplied ELF symbol tables before opening the output file. Only its own two emitted helpers are exempt. The build driver explicitly exits on generator failure, required because it uses set -uo pipefail without -e. Unrelated missing overlays do not block generation when all requested symbols have classification evidence. The existing func_-named symbol rule for symbols present in ELF remains unchanged; no new type inference was added.

Validation: python3 tools/tests/test_gen_port_stubs.py:5 tests PASS using real compiler-produced ELF objects. Two tests failed before repair. Covers correct data/function typing, partial-owner rejection without overwriting existing output, unknown function-style name rejection, known-symbol generation with subset owners, and the real shell driver block stopping before subsequent link work. Real current undefined list plus menu.elf alone rejects without creating output. Complete current overlay set native build PASS:67 function stubs and572 data symbols.

Does not prove freshness of every ELF or completeness of stubbed gameplay. Existing no-ELF fallback and data-sizing policies are unchanged. Large unrelated dirty changes in build_port.sh preserved; no commit or push in this pass.
