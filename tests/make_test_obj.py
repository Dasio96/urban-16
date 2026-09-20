import struct
import os


def write_obj(filename, origin, words):
    dirname = os.path.dirname(filename)
    if dirname:
        os.makedirs(dirname, exist_ok=True)
    with open(filename, "wb") as f:
        f.write(struct.pack(">H", origin))
        for w in words:
            f.write(struct.pack(">H", w & 0xFFFF))


write_obj("test.obj", 0x3000, [
    0xE002, 0xF022, 0xF025,
    ord('U'), ord('R'), ord('B'), ord('A'), ord('N'), 0x0000
])

write_obj("tests/test_alu.obj", 0x3000, [
    0x1225,
    0x1460,
    0x14A5,
    0x56A0,
    0x983F,
    0xF025
])

write_obj("tests/test_branch.obj", 0x3000, [
    0x2004,
    0x0401,
    0xF025,
    0xE000,
    0x0201,
    0xF025,
    0xF025
])

write_obj("tests/test_mem.obj", 0x3000, [
    0xE005,
    0x3005,
    0x2204,
    0x6440,
    0x7441,
    0xAB02,
    0xF025,
    0x3000,
    0x1234
])

write_obj("tests/test_jump.obj", 0x3000, [
    0x4802,
    0xF025,
    0xC1C0,
    0x4000,
    0xF025
])

write_obj("tests/test_illegal.obj", 0x3000, [
    0x8000
])

write_obj("tests/test_traps.obj", 0x3000, [
    0x2004,
    0xF021,
    0xE003,
    0xF022,
    0xF025,
    ord('A'),
    ord('O'), ord('K'), ord('\n'), 0x0000
])
