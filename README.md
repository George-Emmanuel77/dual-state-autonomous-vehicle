# Dual-State Autonomous Vehicle

A four-wheel Arduino vehicle that works in two states: I can drive it from my phone over Bluetooth, or leave it to drive itself and avoid obstacles on its own.

## Hardware

- Arduino Uno
- L298N motor driver (ENA/ENB jumpers removed so speed can be set with PWM)
- 4 DC gear motors on a 3D-printed chassis
- HC-SR04 ultrasonic sensor
- HC-05 Bluetooth module

I planned the wiring in Tinkercad before building it.

## Pin map

| Function | Pin |
|---|---|
| Left forward / backward (IN1 / IN2) | 4 / 5 |
| Right forward / backward (IN3 / IN4) | 6 / 7 |
| ENA (left speed, PWM) | 11 |
| ENB (right speed, PWM) | 3 |
| Ultrasonic trig / echo | 9 / 10 |
| HC-05 | Serial pins 0 / 1 |

The HC-05 shares the Arduino's serial pins, so it has to be unplugged while uploading code.

## How it works

The vehicle starts locked in an emergency-stop state. Commands come in over Bluetooth from a serial terminal app on my phone:

| Command | What it does |
|---|---|
| `G` | Go (release the emergency stop) |
| `S` | Emergency stop |
| `F` / `W` | Fast (full PWM) / slow speed |
| `L` / `R` | Short turn left / right |
| `B` | Reverse |
| `A` / `C` | 90° turn anticlockwise / clockwise |

When there's no manual command, it drives itself. It drives forward and checks the ultrasonic sensor about every 30 ms. If anything comes within 35 cm, it stops, backs up, turns right and carries on.

## What I learned

Two problems looked like code bugs but weren't. The rear motors had a loose connection, and stray wires in front of the ultrasonic sensor were giving false obstacle readings. Testing the wiring and the sensor separately from the code is what found them.
