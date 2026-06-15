# Analog Geiger Counter with Multi-Range Meter Output

An Arduino Nano-based analog Geiger counter using an M4011 tube and a moving-coil meter.
The instrument provides four selectable CPM ranges and a meter test mode, producing a 0–1.1 V
analog output through a filtered PWM signal.

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

## Components Used

| Component | Model / Notes |
|-----------|---------------|
| Microcontroller | Arduino Nano |
| Geiger counter board | RadiationD-v1.1 (CAJOE) with M4011 tube |
| Analog panel meter | Heschen MODEL DH-670, 0–1 V, Class 2.0 |
| Power | 3 × AA battery holder with cable output (included with CAJOE kit) |
| Range switch | 5-position rotary switch (2-pole; second pole unused) |
| Power switch | Single 3 mm toggle switch |
| Interconnect PCB | Hand-soldered Arduino Nano carrier PCB with expanded pin headers |

### Carrier PCB / GND Rail Note

The hand-soldered carrier PCB multiplies available connections on individual pins, used here
primarily to fan out the GND rail. This is functionally equivalent to a dedicated GND bus strip
and can be replaced by one in any future build.

### Power Switch

The 3 mm toggle switch is wired in the position of the original on-board power switch of the
CAJOE board (where the original SMD switch is located). The switch breaks the battery supply
line and turns the entire unit on and off.

---

## ⚠ Hardware Modification — CAJOE Board R19 (Important)

> **Warning:** The RadiationD-v1.1 (CAJOE) board ships with a resistor at position **R19** that
> forms the pulse output divider. On most boards, this resistor is too high in value, which pulls
> the pulse signal so weakly that the Arduino Nano cannot reliably detect counts — resulting in
> zero or heavily suppressed CPM readings even in the presence of a real radiation source.
>
> **Fix:** Replace R19 with a **10 kΩ resistor**. This lowers the output impedance enough for the
> Nano's digital input to trigger correctly.
>
> This is the single most common reason a CAJOE + Arduino Nano build appears not to work.
> Always verify R19 value and replace it before troubleshooting anything else.

---

## Geiger Tube Interface

| Signal | Arduino Nano Pin |
|--------|-----------------|
| Pulse output | D3 |
| +5 V | 5V |
| Ground | GND |

---

## Analog Meter Output

**Meter:** Heschen MODEL DH-670, 0–1 V DC, Class 2.0

The meter is driven by the filtered PWM output on **D9**.

```
Full-scale output voltage: 1.1 V
```

The output is deliberately set to 1.1 V at full scale to compensate for the Class 2.0 tolerance
of the DH-670 (±2 % of full scale = ±20 mV). Driving slightly above the nominal 1.0 V ensures a
clearly pegged end-stop indication at maximum reading, leaving no ambiguity about an overrange
condition.

---

## Range Switch

5-position rotary switch (2-pole; only one pole used). Each active position grounds the
corresponding Arduino pin. All inputs use `INPUT_PULLUP`.

| Position | Pin | Range |
|----------|-----|-------|
| 1 | D4 | 0–100 CPM |
| 2 | D5 | 0–1,000 CPM |
| 3 | D6 | 0–10,000 CPM |
| 4 | D7 | 0–100,000 CPM |
| 5 | D8 | CHECK |

---

# Meter CHECK Mode

Selecting position 5 disconnects the CPM display and runs a continuous meter sweep to verify
meter movement and output circuit integrity.

| Phase | Duration | Output |
|-------|----------|--------|
| Ramp up | 0–5 s | 0 → 1.1 V |
| Ramp down | 5–10 s | 1.1 → 0 V |

The cycle repeats continuously while in CHECK mode.

---

# CPM Processing

Counts are sampled once per second.

**Instantaneous CPM:**
```
InstantCPM = counts_per_second × 60
```

**Filtered display value (exponential moving average):**
```
CPMfiltered = 0.95 × CPMfiltered + 0.05 × CPMinstant
```

This provides:
- Fast response to nearby sources
- Smooth needle movement
- Suppressed single-pulse noise

---

# Output Voltage

For each range:
```
Voltage = 1.1 × (CPMfiltered / FullScale)
```

Correction applied above 0.2 V to compensate for meter non-linearity at low deflections:
```
if Voltage > 0.2 V:
    Voltage += 0.05 V
```

Maximum output is clamped to:
```
1.1 V
```

---

# Battery Voltage Compensation

The Arduino continuously measures its own supply voltage via the internal reference.
PWM duty cycle is automatically scaled:

```
PWM = Voltage × 255 / Vcc
```

This keeps meter calibration nearly constant across:
- USB power (5.0 V)
- Regulated external 5 V supply
- 3 × AA battery pack (voltage sags with discharge)

---

# Serial Plotter

The sketch outputs one line per second in the format:

```
CPM:34.2 Voltage:0.423 Scale:100
```

Open **Tools → Serial Plotter** in the Arduino IDE to view live graphs of CPM, output voltage,
and active range simultaneously.

---

# Pin Assignments

| Function | Pin |
|----------|-----|
| Geiger pulse input | D3 |
| Analog meter output (PWM) | D9 |
| Range: 100 CPM | D4 |
| Range: 1,000 CPM | D5 |
| Range: 10,000 CPM | D6 |
| Range: 100,000 CPM | D7 |
| CHECK mode | D8 |

---

# Recommended Components

- Arduino Nano
- RadiationD-v1.1 (CAJOE) Geiger board with M4011 tube
- Heschen MODEL DH-670 0–1 V Class 2.0 analog panel meter
- 5-position rotary switch (1- or 2-pole)
- 3 mm toggle switch (power)
- 3 × AA battery holder with cable
- Hand-soldered carrier PCB or dedicated GND bus strip

---

# Notes

The analog output uses PWM on D9 with an external RC low-pass filter. For lower ripple and
improved linearity, a DAC such as the MCP4725 (I²C, 12-bit) can be substituted for the PWM
output stage.

This instrument is intended for educational and experimental use. It is not calibrated for
radiation protection or dosimetry purposes and should not be used as a substitute for certified
dosimetry equipment.
