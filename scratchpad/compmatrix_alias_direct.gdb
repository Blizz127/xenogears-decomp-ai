set pagination off
set confirm off
set debuginfod enabled off

break FieldMain
commands
  silent
  set $ram = (unsigned char*)g_PsxRam
  set $m0 = $ram + 0xe0000
  set $m1 = $ram + 0xe0100
  set $out = $ram + 0xe0200

  # Identity matrices, m0.t=(100,200,300), m1.t=(10,20,30).
  set {short}($m0+0)=4096
  set {short}($m0+2)=0
  set {short}($m0+4)=0
  set {short}($m0+6)=0
  set {short}($m0+8)=4096
  set {short}($m0+10)=0
  set {short}($m0+12)=0
  set {short}($m0+14)=0
  set {short}($m0+16)=4096
  set {int}($m0+20)=100
  set {int}($m0+24)=200
  set {int}($m0+28)=300
  set {short}($m1+0)=4096
  set {short}($m1+2)=0
  set {short}($m1+4)=0
  set {short}($m1+6)=0
  set {short}($m1+8)=4096
  set {short}($m1+10)=0
  set {short}($m1+12)=0
  set {short}($m1+14)=0
  set {short}($m1+16)=4096
  set {int}($m1+20)=10
  set {int}($m1+24)=20
  set {int}($m1+28)=30

  call (void)CompMatrix((MATRIX*)$m0, (MATRIX*)$m1, (MATRIX*)$out)
  printf "COMP_DISTINCT t=[%d,%d,%d]\n", *(int*)($out+20), *(int*)($out+24), *(int*)($out+28)
  if *(int*)($out+20) != 110 || *(int*)($out+24) != 220 || *(int*)($out+28) != 330
    printf "COMP_ALIAS_FAIL distinct output\n"
    quit 1
  end

  # Alias output to m0 with zero m1 translation. Retail preserves m0.t.
  set {int}($m0+20)=110
  set {int}($m0+24)=220
  set {int}($m0+28)=330
  set {int}($m1+20)=0
  set {int}($m1+24)=0
  set {int}($m1+28)=0
  call (void)CompMatrix((MATRIX*)$m0, (MATRIX*)$m1, (MATRIX*)$m0)
  printf "COMP_M0_ALIAS_ZERO t=[%d,%d,%d]\n", *(int*)($m0+20), *(int*)($m0+24), *(int*)($m0+28)
  if *(int*)($m0+20) != 110 || *(int*)($m0+24) != 220 || *(int*)($m0+28) != 330
    printf "COMP_ALIAS_FAIL m0 alias with zero m1 translation\n"
    quit 1
  end

  # Reset both translations and alias output to m0 with nonzero m1.t.
  set {int}($m0+20)=100
  set {int}($m0+24)=200
  set {int}($m0+28)=300
  set {int}($m1+20)=10
  set {int}($m1+24)=20
  set {int}($m1+28)=30
  call (void)CompMatrix((MATRIX*)$m0, (MATRIX*)$m1, (MATRIX*)$m0)
  printf "COMP_M0_ALIAS_NONZERO t=[%d,%d,%d]\n", *(int*)($m0+20), *(int*)($m0+24), *(int*)($m0+28)
  if *(int*)($m0+20) != 110 || *(int*)($m0+24) != 220 || *(int*)($m0+28) != 330
    printf "COMP_ALIAS_FAIL m0 alias with nonzero m1 translation\n"
    quit 1
  end

  # Reset both and alias output to m1. This ordering remains safe.
  set {int}($m0+20)=100
  set {int}($m0+24)=200
  set {int}($m0+28)=300
  set {int}($m1+20)=10
  set {int}($m1+24)=20
  set {int}($m1+28)=30
  call (void)CompMatrix((MATRIX*)$m0, (MATRIX*)$m1, (MATRIX*)$m1)
  printf "COMP_M1_ALIAS t=[%d,%d,%d]\n", *(int*)($m1+20), *(int*)($m1+24), *(int*)($m1+28)
  if *(int*)($m1+20) != 110 || *(int*)($m1+24) != 220 || *(int*)($m1+28) != 330
    printf "COMP_ALIAS_FAIL m1 alias output\n"
    quit 1
  end
  printf "COMP_ALIAS_SENTINEL\n"
  quit
end

run
