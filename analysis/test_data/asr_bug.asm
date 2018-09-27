_start:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
  adrp x0, data
  add x0, x0, #0x123
  asr x0, x0, 3
  ret

data: