# Embedded Network System

Listen to UART via C232HM Cable
```c
$ screen /dev/{console_port} 115200
// example
$ sudo screen /dev/ttyUSB0 9600
```

Flash Atmegga via USB
```c
$ avrdude -c usbasp -p m644p -U flash:w:{program}.hex
```
