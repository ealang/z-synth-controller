import spidev

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 20000
spi.mode = 0

print(spi.xfer(range(16 + 2)))
