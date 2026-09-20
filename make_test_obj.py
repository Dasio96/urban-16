import struct

origin = 0x3000

code = [
    0xE002,
    0xF022,
    0xF025,
]

message = "URBAN"
data = [ord(c) for c in message]

with open("test.obj","wb") as f:
    f.write(struct.pack(">H", origin))
    for word in code + data:
        f.write(struct.pack(">H", word))
