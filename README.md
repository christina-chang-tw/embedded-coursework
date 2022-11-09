# Embedded Network System

Listen to UART via C232HM Cable
```c
$ screen /dev/{console_port} 115200
// example - baud rate 9600 with UTF-8 encoding
$ sudo screen /dev/ttyUSB0 9600 -U
```

Flash Atmegga via USB
```c
$ avrdude -c usbasp -p m644p -U flash:w:{program}.hex
```

For UART communication 
| Atmegga Port | C232HM Cable |
| ------------ | ------------ |
| PD0 (RX)     | Orange (TX)  |
| PD1 (TX)     | Yellow (RX)  |


App circuit setup
- push-button switch
- 1kohm resistor
- 220nF capacitor
The interrupt pins are pulled high; therefore, to trigger the interrupt cycle, those pins have to be pulled low. Each interrupt pin will be connected to a 1kohm resistor. One side of the switch is connected to the resistor and the other side connected to GND. A capacitor is placed across the switch to debounce the switch.
