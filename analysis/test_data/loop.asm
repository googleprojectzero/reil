_start:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
  mov x0, #16
loop:
  subs x0, x0, #1  
  b.ne loop
  ldp x29, x30, [sp, #0x10]
  add sp, sp, #0x20
  ret
