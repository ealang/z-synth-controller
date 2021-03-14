import serial

# https://pythonhosted.org/pyserial/pyserial_api.html#classes
ser = serial.Serial(
    '/dev/ttyUSB0',
    baudrate=50000,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.005,
)

while True:
    data = ser.read(18)
    if data:
        data_fmt = " ".join(hex(_) for _ in data)
        print(data_fmt)
