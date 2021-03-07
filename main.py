import spidev
import time

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 50000
spi.mode = 0

i = 0
ts = time.time()
while True:
    i += 1
    data = spi.xfer(range(16 + 2))
    if data[0] != 0xEE or data[-1] != 0xEE:
        print(i, data)
    if i % 1000 == 0:
        print(i, time.time() - ts)
