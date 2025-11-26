import serial
bin_path = r"D:\STM32Cubemx_Project\OTA\application2.bin"
version = "1.2"
port = "COM9"
baud = 460800

with open(bin_path, "rb") as f:
    buf = f.read()
    size = len(buf)
    if size % 1024 != 0:
        padding = 1024 - (size % 1024)
        buf += b'\xff' * padding
        size += padding

print("文件大小：", size / 1024, "KB")

ser = serial.Serial(port, baud, timeout=1)


while True:
    chunk = ser.read(1)
    if chunk == b"v":
        major, minor = version.split('.')
        version_bytes = bytes([int(major), int(minor)])
        ser.write(version_bytes)
        break


while True:
    chunk = ser.read(1)
    if chunk == b"x":
        high_byte = (size // 1024) // 256
        low_byte = (size // 1024) % 256
        send_bytes = bytes([high_byte, low_byte])
        ser.write(send_bytes)
        break



index = 0
while index < size:
    chunk = ser.read(1)
    if chunk == b"y":
        temp = buf[index:index + 1024]
        ser.write(temp)
        index += 1024

       

print("传输完成")
ser.close()