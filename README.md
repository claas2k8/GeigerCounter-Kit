# Analog Geiger Counter with Multi-Range Meter Output

An Arduino Nano-based analog Geiger counter using an M4011 tube and a moving-coil meter. The instrument provides four selectable CPM ranges and a meter test mode, producing a 0–1.1 V analog output through a filtered PWM signal.

---

# Features

- M4011 Geiger-Müller tube support
- Real-time response with exponential filtering
- Smooth analog meter indication
- Four selectable ranges
- Meter CHECK function
- Battery-voltage compensated output
- 1.1 V full-scale output
- Automatic +0.05 V output correction above 0.2 V
- Serial Plotter support
- Pulse debounce protection

---

# Hardware

## Geiger Tube Interface

| Signal | Arduino Nano |
|----------|----------|
| Pulse Output | D3 |
| +5 V | 5V |
| Ground | GND |

---

## Analog Meter Output

The output on D9 
```

Full-scale meter voltage:

```
1 V

but to combat meter inacuracy an full scale max reading is then dispalyed as 1.1V to enshure an pegged indication.
```

---

## Range Switch

The switch grounds one pin at a time.

| Position | Pin | Range |
|-----------|-----|------|
| 1 | D4 | 0–100 CPM |
| 2 | D5 | 0–1000 CPM |
| 3 | D6 | 0–10,000 CPM |
| 4 | D7 | 0–100,000 CPM |
| 5 | D8 | CHECK |

All inputs use:

```cpp
INPUT_PULLUP
```

---

# Meter CHECK Mode

Selecting position 5 disconnects the CPM display and instead performs a continuous meter test.

### Cycle

| Time | Output |
|--------|------|
| 0–5 s | 0 → 1.1 V |
| 5–10 s | 1.1 → 0 V |

The cycle repeats continuously while in CHECK mode.

---

# CPM Processing

Counts are sampled once per second.

Instantaneous CPM:

```
Instant CPM = Counts/s × 60
```

The display value is filtered using an exponential moving average:

```
CPMfiltered = 0.95 × CPMfiltered + 0.05 × CPMinstant
```

This provides:

- Fast response to nearby sources
- Smooth needle movement
- No excessive jumping

---

# Output Voltage

For each range:

```
Voltage = 1.1 × (CPM / FullScale)
```

An additional correction is applied:

```
if Voltage > 0.2 V
    Voltage += 0.05 V
```

Maximum output:

```
1.1 V
```

---

# Battery Compensation

The Arduino continuously measures its own supply voltage.

PWM output is automatically adjusted:

```
PWM = Voltage × 255 / Vcc
```

This keeps the meter calibration nearly constant whether powered from:

- USB
- External 5 V supply
- 3×AAA battery pack

---

# Serial Plotter

The sketch outputs:

- CPM
- Voltage
- Scale

Example:

```
CPM:34.2 Voltage:0.423 Scale:100
```

Open:

```
Tools → Serial Plotter
```

to view live graphs.

---

# Pin Assignments

| Function | Pin |
|-----------|-----|
| Geiger pulse input | D3 |
| Analog meter output | D9 |
| 100 CPM range | D4 |
| 1000 CPM range | D5 |
| 10,000 CPM range | D6 |
| 100,000 CPM range | D7 |
| CHECK mode | D8 |

---

# Recommended Components

- Arduino Nano
- M4011 Geiger tube interface board
- Analog moving-coil meter
- 10 kΩ resistor
- 10 µF capacitor
- 5-position rotary switch
- 3×AAA battery holder or regulated 5 V supply

---

# Notes

The analog output uses PWM and an RC filter. For improved accuracy and lower ripple, a DAC such as an MCP4725 can be substituted.

This instrument is intended for educational and experimental use and is not calibrated for radiation protection or dosimetry applications.
