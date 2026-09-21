set pagination off
set confirm off
handle SIGTERM nostop noprint pass
set $n7d = 0
break func_801E7D14
commands
  silent
  set $n7d = $n7d + 1
  if $n7d == 3 || $n7d == 120
    set $s = 0
    while $s < 4
      set $o = (unsigned char*)(unsigned long)D_801E8670[$s]
      set $r = (unsigned char*)(unsigned long)*(unsigned int*)($o+4)
      set $n1 = $r + 0x7c
      set $n7 = $r + 7*0x7c
      printf "[T] call=%d slot=%d obj1C=%d tbl=%d root.t=(%d,%d,%d) root.L0=(%d,%d,%d) n1.W0=(%d,%d,%d) n1.Wt=(%d,%d,%d) n7.W0=(%d,%d,%d) n7.Wt=(%d,%d,%d) n1.f45=(%d,%d)\n", $n7d, $s, *(short*)($o+0x1c), D_800B220C[$s], *(int*)($r+0x5c), *(int*)($r+0x60), *(int*)($r+0x64), *(short*)($r+0xc), *(short*)($r+0xe), *(short*)($r+0x10), *(short*)($n1+0x2c), *(short*)($n1+0x2e), *(short*)($n1+0x30), *(int*)($n1+0x40), *(int*)($n1+0x44), *(int*)($n1+0x48), *(short*)($n7+0x2c), *(short*)($n7+0x2e), *(short*)($n7+0x30), *(int*)($n7+0x40), *(int*)($n7+0x44), *(int*)($n7+0x48), $n1[4], $n1[5]
      set $s = $s + 1
    end
  end
  continue
end
run
quit
