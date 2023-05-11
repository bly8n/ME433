from ulab import numpy as np
import board
import time
import digitalio
import analogio

some_led = digitalio.DigitalInOut(board.GP16)
some_led.direction = digitalio.Direction.OUTPUT

some_button = digitalio.DigitalInOut(board.GP15)
some_button.direction = digitalio.Direction.INPUT

built_in_led = digitalio.DigitalInOut(board.LED)
built_in_led.direction = digitalio.Direction.OUTPUT

adc_pin_a0 = analogio.AnalogIn(board.A0)
t = np.linspace(0, 2 * np.pi, 1024)
freq1 = 1  # Frequency of the first sine wave
freq2 = 3  # Frequency of the second sine wave
freq3 = 6  # Frequency of the third sine wave
y1 = 1.5 * np.sin(freq1 * t)
y2 = 1.5 * np.sin(freq2 * t)
y3 = 1.5 * np.sin(freq3 * t)
y_sum = y1 + y2 + y3
Y = np.fft.fft(y_sum)
for value in Y[1]:
   # Generate the three sine waves
   	
   print("("+str(value)+",)")
   if (adc_pin_a0.value > 14000):
       some_led.value = 1
   else:
       some_led.value = 0
   if (some_button.value == 0):
       built_in_led.value = 1
   else:
       built_in_led.value = 0
   time.sleep(.05) # delay in seconds