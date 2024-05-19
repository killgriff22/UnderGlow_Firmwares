import serial

ser = serial.Serial("/dev/tty.usbserial-1410")
ser.baudrate = 115200

while True:
    if not ser.in_waiting < 3:
        continue
    command = ser.read()
    address = ser.read()
    data = ser.read()
    match command:
        case b'\x01':
            print(f"read from {address}")
            match address:
                case b'\x00':
                    print("R")
                    ser.write(0xFF)
                case b'\x01':
                    print("G")
                    ser.write(0x00)
                case b'\x02':
                    print("B")
                    ser.write(0x00)
                case b'\x03':
                    print("Pattern")
                    ser.write(0x01)
                case b'\x04':
                    print("Brightness")
                    ser.write(0xFF)
                case b'\x05':
                    print("Speed")
                    ser.write(0x00)
