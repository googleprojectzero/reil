_start:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
  mov x0, #0x123
  subs x0, x0, #0x123
  b.eq done
  add x0, x0, #0x123

done:
  ret
