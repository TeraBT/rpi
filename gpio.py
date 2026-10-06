from gpiozero import DigitalOutputDevice
from time import sleep

# r = LED(17)
# g = LED(18)
# b = LED(27)

serial_input = DigitalOutputDevice(17)
shift_clock = DigitalOutputDevice(18)
storage_clock = DigitalOutputDevice(27)

t = 0.2

for i in range(8):

    serial_input.off()

    shift_clock.on()
    shift_clock.off()

storage_clock.on()
storage_clock.off()

sleep(1)

# while True:

#     for i in range(7):
#         serial_input.value = 1 - (i % 2)

#         shift_clock.on()
#         shift_clock.off()

#     storage_clock.on()
#     storage_clock.off()
#     sleep(1)

#     for i in range(7):
#         serial_input.value = i % 2

#         shift_clock.on()
#         shift_clock.off()

#     storage_clock.on()
#     storage_clock.off()
#     sleep(1)

while True:

    serial_input.on()

    shift_clock.on()
    shift_clock.off()

    storage_clock.on()
    storage_clock.off()
    sleep(0.05)

    for i in range(7):
        serial_input.off()

        shift_clock.on()
        shift_clock.off()

        storage_clock.on()
        storage_clock.off()
        sleep(0.05)

# for i in range(8):

#     serial_input.value = i % 2

#     shift_clock.on()
#     shift_clock.off()


# storage_clock.on()
# storage_clock.off()

# sleep(1)


# while True:
#     r.on()
#     g.on()
#     b.on()
#     sleep(t)
#     r.off()
#     g.off()
#     b.off()
#     sleep(t)

# while True:
#     r.on()
#     sleep(t)
#     r.off()
#     g.on()
#     sleep(t)
#     g.off()
#     b.on()
#     sleep(t)
#     b.off()


# while True:
#     for i in range(10):
#         pin.on()
#         sleep(0.1)
#         pin.off()
#         sleep(0.1)

#     pin.on()
#     sleep(1)
#     pin.off()
