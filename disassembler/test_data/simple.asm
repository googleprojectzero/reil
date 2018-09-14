_start:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
  bl  function
  cbz x0, label
  nop
label:
  ldp x29, x30, [sp, #0x10]
  add sp, sp, #0x20
  ret

function:
  ret
