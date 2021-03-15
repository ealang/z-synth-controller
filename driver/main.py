import serial
import time

# https://pythonhosted.org/pyserial/pyserial_api.html#classes
ser = serial.Serial(
    '/dev/ttyUSB0',
    baudrate=50000,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.005,
)

t0 = time.time()
while True:
    data = ser.read(18)
    if data:
        delta = (time.time() - t0) * 1000
        hex_fmt = " ".join("{:02x}".format(_) for _ in data)
        try:
            str_fmt = data.decode("ascii")
        except:
            str_fmt = ""
        print(f"{hex_fmt} | {str_fmt} | {delta:.1f} ms")
        t0 = time.time()
