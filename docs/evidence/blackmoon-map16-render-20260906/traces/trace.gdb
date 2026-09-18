set pagination off
set confirm off
set print pretty off
handle SIGTERM nostop noprint pass
define ip8
  printf "  ip=%u bytes:", g_FieldScriptVMCurActor->scriptInstructionPointer
  set $b = (unsigned char*)g_FieldScriptVMCurScriptData + g_FieldScriptVMCurActor->scriptInstructionPointer
  set $i = 0
  while $i < 20
    printf " %02x", $b[$i]
    set $i = $i + 1
  end
  printf "\n"
end
break func_800A0FD8
commands
  silent
  printf "[T] A0FD8 actor=%d ", D_800AFD1C
  ip8
  continue
end
break func_801E742C
commands
  silent
  printf "[T] 742C slot=%d ids=%u %u %u %u\n", $rdi, D_800B21DC[0], D_800B21DC[1], D_800B21DC[2], D_800B21DC[3]
  continue
end
break func_80099214
commands
  silent
  printf "[T] OP57 actor=%d pos=(%d,%d,%d) ", D_800AFD1C, g_FieldScriptVMCurActor->position.vx>>16, g_FieldScriptVMCurActor->position.vy>>16, g_FieldScriptVMCurActor->position.vz>>16
  ip8
  continue
end
break func_8008FB98
commands
  silent
  printf "[T] FB98 camMode=1 actor=%d ", D_800AFD1C
  ip8
  continue
end
break FieldScriptSetCameraPosMovementDest
commands
  silent
  printf "[T] EYEDEST actor=%d ", D_800AFD1C
  ip8
  continue
end
break FieldScriptSetCameraTargetMovementDest
commands
  silent
  printf "[T] ATDEST actor=%d ", D_800AFD1C
  ip8
  continue
end
break FieldScriptSetCameraPosMovementFrom
commands
  silent
  printf "[T] EYEFROM actor=%d ", D_800AFD1C
  ip8
  continue
end
break FieldScriptSetCameraTargetMovementFrom
commands
  silent
  printf "[T] ATFROM actor=%d ", D_800AFD1C
  ip8
  continue
end
break FieldScriptStartCameraMovement
commands
  silent
  printf "[T] CAMSTART actor=%d eyeFrom=(%d,%d,%d) eyeTo=(%d,%d,%d) atFrom=(%d,%d,%d) atTo=(%d,%d,%d) ", D_800AFD1C, g_CamEyeMovementFrom.vx>>16, g_CamEyeMovementFrom.vy>>16, g_CamEyeMovementFrom.vz>>16, g_CamEyeMovementTo.vx>>16, g_CamEyeMovementTo.vy>>16, g_CamEyeMovementTo.vz>>16, g_CamAtMovementFrom.vx>>16, g_CamAtMovementFrom.vy>>16, g_CamAtMovementFrom.vz>>16, g_CamAtMovementTo.vx>>16, g_CamAtMovementTo.vy>>16, g_CamAtMovementTo.vz>>16
  ip8
  continue
end
break func_8008AEC8
commands
  silent
  printf "[T] LIGHTCOL actor=%d ", D_800AFD1C
  ip8
  continue
end
break func_8008AFD8
commands
  silent
  printf "[T] LIGHTDIR actor=%d ", D_800AFD1C
  ip8
  continue
end
run
quit
