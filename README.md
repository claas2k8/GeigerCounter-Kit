# Analog Geiger Counter with Multi-Range Meter Output

## Overview

This project converts the output of an M4011 Geiger-Müller tube into a proportional analog meter indication using an Arduino Nano and a moving-coil meter.

The instrument uses a 60-second rolling CPM average to provide stable readings without excessive needle movement. Four switch-selectable ranges are provided, together with a battery check position.

The analog output is generated on pin D9 using PWM and a low-pass RC filter. The meter is calibrated for a full-scale output of 1.1 V.

---

## Features

* M4011 Geiger tube support
* 60-second rolling CPM calculation
* Stable analog needle indication
* Four selectable CPM ranges
* Battery check mode
* 1.1 V full-scale analog output
* Additional +0.05 V correction above 0.2 V
* Serial monitor output
* 10 ms pulse debounce

---

## Hardware Connections

### Geiger Module

| M4011 Interface | Arduino Nano |
| --------------- | ------------ |
| Pulse Output    | D3           |
| +5 V            | 5 V          |
| Ground          | GND          |

---

### Analog Meter Output

| Arduino Nano | Connection             |
| ------------ | ---------------------- |
| D9           | 10 kΩ resistor         |
| Output node  | Analog meter input     |
| Output node  | 10 µF capacitor to GND |

The resistor and capacitor form a low-pass filter that converts the PWM output into a smooth DC voltage.

---

### Range Switch

The range switch grounds one pin at a time.

| Position | Pin | Range         |
| -------- | --- | ------------- |
| 1        | D4  | 0–100 CPM     |
| 2        | D5  | 0–1000 CPM    |
| 3        | D6  | 0–10,000 CPM  |
| 4        | D7  | 0–100,000 CPM |
| 5        | D10 | Battery Check |

All switch inputs use INPUT_PULLUP.

---

## Battery Check Mode

When position 5 is selected (D10 grounded), the meter no longer displays CPM.

Instead:

* 0 V supply → 0 V meter output
* 5.0 V supply → 1.1 V meter output (full scale)

This allows the meter to function similarly to classic survey meters.

---

## Output Calibration

The meter is calibrated so that:

| Meter Output | Scale Reading |
| ------------ | ------------- |
| 0 V          | Zero          |
| 0.55 V       | Half scale    |
| 1.1 V        | Full scale    |

An additional correction of +0.05 V is applied whenever the output exceeds 0.2 V.

---

## CPM Averaging

Counts are accumulated in sixty 1-second bins.

The displayed CPM is the sum of the previous 60 seconds:

CPM = counts collected during the last 60 seconds

This approach prevents unrealistic jumps caused by individual pulses and provides smooth meter operation.

---

## Serial Output

Example:

```
CPM: 34 | Range: 0-100 | Output: 0.424 V
```

Battery check mode:

```
CHECK MODE | Vcc = 4.97 V
```

---

## Pin Assignments

| Function            | Pin |
| ------------------- | --- |
| Geiger Pulse Input  | D3  |
| Analog Meter Output | D9  |
| 100 CPM Range       | D4  |
| 1000 CPM Range      | D5  |
| 10,000 CPM Range    | D6  |
| 100,000 CPM Range   | D7  |
| Battery Check       | D10 |

---

## Recommended Supply

* Arduino Nano
* 5 V regulated supply
* M4011 Geiger tube interface board
* Analog meter movement requiring 1.1 V full-scale indication

---

## Notes

The analog output is PWM-based and relies on the RC filter for smoothing. For higher accuracy and lower ripple, a DAC such as the MCP4725 may be substituted.

This instrument is intended for educational and experimental use and is not calibrated for radiation protection or dosimetry applications.
