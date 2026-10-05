from gpiozero import LED
from time import sleep

pin = LED(17)

while True:
    for i in range(10):
        pin.on()
        sleep(0.1)
        pin.off()
        sleep(0.1)

    pin.on()
    sleep(1)
    pin.off()
