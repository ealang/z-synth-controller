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
        data_fmt = " ".join(hex(_) for _ in data)
        print(data_fmt, f"({delta:.1f} ms)")
        t0 = time.time()
