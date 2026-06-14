//This is an code for a Arduino Nano conected to an CAJOE Radiation V1.1 PCB using an MJ4011 Tube and an 5 position rotary switch. As an display for CPM i am using an DH-670 Voltmetre with an range of 0-1V.
// Feel free to modify and use this code 

const byte INT_PIN = 3;
const byte PWM_PIN = 9;

// Range switch pins
const byte RANGE100_PIN    = 4;
const byte RANGE1000_PIN   = 5;
const byte RANGE10000_PIN  = 6;
const byte RANGE100000_PIN = 7;
const byte CHECK_PIN       = 8;

volatile unsigned long totalPulses = 0;

unsigned long lastSecond = 0;
unsigned long pulsesLastSecond = 0;

// Exponential filter (medium response)
float cpmFiltered = 0.0;

// measured supply voltage
float vccVoltage = 5.0;

void onPulse() {
  static unsigned long last = 0;
  unsigned long now = micros();

  if (now - last < 10000UL)
    return;

  last = now;
  totalPulses++;
}

// Measure Vcc (mV)
long readVcc() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);

  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));

  long result = ADCL;
  result |= ADCH << 8;

  result = 1125300L / result;
  return result;
}

void setup() {
  Serial.begin(9600);

  pinMode(INT_PIN, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT);

  pinMode(RANGE100_PIN, INPUT_PULLUP);
  pinMode(RANGE1000_PIN, INPUT_PULLUP);
  pinMode(RANGE10000_PIN, INPUT_PULLUP);
  pinMode(RANGE100000_PIN, INPUT_PULLUP);

  pinMode(CHECK_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), onPulse, FALLING);

  lastSecond = millis();

  Serial.println(F("Geiger Meter Ready (Battery Compensated)"));
}

void loop() {

  unsigned long now = millis();

  // ---- update Vcc every 2 seconds ----
  static unsigned long lastVccCheck = 0;
  if (now - lastVccCheck > 2000) {
    vccVoltage = readVcc() / 1000.0;
    lastVccCheck = now;
  }

  // ---- 1-second CPM update ----
  if (now - lastSecond >= 1000) {

    noInterrupts();
    unsigned long currentTotal = totalPulses;
    interrupts();

    unsigned long pulsesThisSecond = currentTotal - pulsesLastSecond;
    pulsesLastSecond = currentTotal;

    float instantCPM = pulsesThisSecond * 60.0;

    cpmFiltered = 0.95 * cpmFiltered + 0.05 * instantCPM;

    lastSecond += 1000;
  }

  float voltage = 0.0;

  // ---------------- CHECK MODE ----------------
  if (!digitalRead(CHECK_PIN)) {

    float t = (millis() % 10000UL) / 1000.0;

    if (t < 5.0)
      voltage = 1.1 * (t / 5.0);
    else
      voltage = 1.1 * ((10.0 - t) / 5.0);

    Serial.print("CHECK MODE | V = ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  }

  // ---------------- NORMAL MODE ----------------
  else {

    float fullScale = 100.0;

    if (!digitalRead(RANGE100_PIN))
      fullScale = 100.0;

    else if (!digitalRead(RANGE1000_PIN))
      fullScale = 1000.0;

    else if (!digitalRead(RANGE10000_PIN))
      fullScale = 10000.0;

    else if (!digitalRead(RANGE100000_PIN))
      fullScale = 100000.0;

    float cpm = cpmFiltered;

    voltage = 1.1 * (cpm / fullScale);

    if (voltage > 0.2)
      voltage += 0.05;

    if (voltage > 1.1)
      voltage = 1.1;

    Serial.print("CPM: ");
    Serial.print(cpm);

    Serial.print(" | Range: 0-");
    Serial.print((unsigned long)fullScale);

    Serial.print(" | V: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  }

  // ---- BATTERY-COMPENSATED PWM OUTPUT ----
  int pwmValue = (int)(voltage * 255.0 / vccVoltage);
  analogWrite(PWM_PIN, pwmValue);

  delay(100);
}