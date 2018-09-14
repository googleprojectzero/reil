_start:
  sub sp, sp, #0x20
  stp d0, d1, [sp, #0x10]
  add x29, sp, #0x10
  ret

function1:
  sub sp, sp, #0x20
  stp d0, d1, [sp, #0x10]!
  add x29, sp, #0x10
  ret

function2:
  stp d0, d1, [sp, #0x10]
  add x29, sp, #0x10
  ret

function3:
  stp d0, d1, [sp, #0x10]!
  add x29, sp, #0x10
  ret

function4:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  stp d0, d1, [sp, #0x10]
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
  ret
