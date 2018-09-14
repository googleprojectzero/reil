_start:
  sub sp, sp, #0x20
  stp x29, x30, [sp, #0x10]
  add x29, sp, #0x10
split:
  cbz x0, split
  ret