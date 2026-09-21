set pagination off
set confirm off
handle SIGTERM nostop noprint pass
define ip8
  printf "  ip=%u bytes:", g_FieldScriptVMCurActor->scriptInstructionPointer
  set $b = (unsigned char*)g_FieldScriptVMCurScriptData + g_FieldScriptVMCurActor->scriptInstructionPointer
  set $i = 0
  while $i < 16
    printf " %02x", $b[$i]
    set $i = $i + 1
  end
  printf "\n"
end
set $n98 = 0
break func_80098CAC
commands
  silent
  set $n98 = $n98 + 1
  if $n98 <= 40
    printf "[T] 98CAC actor=%d arg0=%d pos=(%d,%d,%d) unk102=%d ", D_800AFD1C, $rdi, g_FieldScriptVMCurActor->position.vx>>16, g_FieldScriptVMCurActor->position.vy>>16, g_FieldScriptVMCurActor->position.vz>>16, g_FieldScriptVMCurActor->unk102
    ip8
  end
  continue
end
set $n7d = 0
break func_801E7D14
commands
  silent
  set $n7d = $n7d + 1
  if $n7d == 3
    printf "[T] LLM D_800B221C:"
    set $i = 0
    while $i < 9
      printf " %d", ((short*)&D_800B221C)[$i]
      set $i = $i + 1
    end
    printf "\n[T] LCM D_800B223C:"
    set $i = 0
    while $i < 9
      printf " %d", ((short*)&D_800B223C)[$i]
      set $i = $i + 1
    end
    printf "\n[T] fieldLLM D_80059F64:"
    set $i = 0
    while $i < 9
      printf " %d", ((short*)&D_80059F64)[$i]
      set $i = $i + 1
    end
    printf "\n[T] fieldLCM D_80059F84:"
    set $i = 0
    while $i < 9
      printf " %d", ((short*)&D_80059F84)[$i]
      set $i = $i + 1
    end
    printf "\n[T] back=(%d,%d,%d) aux86A0=%08x %08x acc8640=%d\n", (int)*(unsigned char*)&D_800B225C, (int)*(unsigned char*)&D_800B225D, (int)*(unsigned char*)&D_800B225E, ((unsigned int*)D_801E86A0)[0], ((unsigned int*)D_801E86A0)[1], D_801E8640
    printf "[T] light0 dir=(%d,%d,%d) col=(%d,%d,%d) light1 dir=(%d,%d,%d) col=(%d,%d,%d)\n", *(int*)((char*)&g_Scene+0x138), *(int*)((char*)&g_Scene+0x13c), *(int*)((char*)&g_Scene+0x140), *(unsigned short*)((char*)&g_Scene+0x144), *(unsigned short*)((char*)&g_Scene+0x146), *(unsigned short*)((char*)&g_Scene+0x148), *(int*)((char*)&g_Scene+0x14c), *(int*)((char*)&g_Scene+0x150), *(int*)((char*)&g_Scene+0x154), *(unsigned short*)((char*)&g_Scene+0x158), *(unsigned short*)((char*)&g_Scene+0x15a), *(unsigned short*)((char*)&g_Scene+0x15c)
  end
  continue
end
run
quit
