set pagination off
set confirm off
handle SIGTERM nostop noprint pass
set $h = 0
watch *(short*)((unsigned char*)&g_FieldBss_800B2174 + 0xd8)
commands
  silent
  set $h = $h + 1
  if $h <= 6
    printf "[T] W224C#%d val=%d from:\n", $h, *(short*)((unsigned char*)&g_FieldBss_800B2174 + 0xd8)
    bt 4
  end
  continue
end
set $g = 0
watch *(short*)((unsigned char*)&g_FieldBss_800B2174 + 0xb8)
commands
  silent
  set $g = $g + 1
  if $g <= 6
    printf "[T] W222C#%d val=%d from:\n", $g, *(short*)((unsigned char*)&g_FieldBss_800B2174 + 0xb8)
    bt 4
  end
  continue
end
run
quit
