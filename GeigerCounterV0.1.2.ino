// Geiger Counter → Analog Meter Output (0–1.1V)
// M4011 input on D3
// Analog output on D9
// Check position on D10

const byte INT_PIN = 3;
const byte PWM_PIN = 9;

// Range switch pins
const byte RANGE100_PIN    = 4;
const byte RANGE1000_PIN   = 5;
const byte RANGE10000_PIN  = 6;
const byte RANGE100000_PIN = 7;
const byte CHECK_PIN       = 8;

volatile unsigned long totalPulses = 0;

// 60-second rolling buffer
unsigned int pulseBins[60];
byte currentBin = 0;

unsigned long lastSecond = 0;
unsigned long pulsesLastSecond = 0;

// Read Arduino supply voltage (mV)
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

void onPulse() {
  static unsigned long last = 0;

  unsigned long now = micros();

  if (now - last < 10000UL)
    return; // 10 ms debounce

  last = now;

  totalPulses++;
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

  for (int i = 0; i < 60; i++)
    pulseBins[i] = 0;

  lastSecond = millis();

  Serial.println(F("Analog Geiger Meter"));
}

void loop() {

  unsigned long now = millis();

  // ---- Update 1-second bins ----
  if (now - lastSecond >= 1000) {

    noInterrupts();
    unsigned long currentTotal = totalPulses;
    interrupts();

    pulseBins[currentBin] = currentTotal - pulsesLastSecond;
    pulsesLastSecond = currentTotal;

    currentBin++;

    if (currentBin >= 60)
      currentBin = 0;

    lastSecond += 1000;
  }

  // ---- Calculate CPM ----
  unsigned long cpm = 0;

  for (int i = 0; i < 60; i++)
    cpm += pulseBins[i];

  float voltage = 0.0;

  // ---------------- CHECK POSITION ----------------
  if (!digitalRead(CHECK_PIN)) {

    float vcc = readVcc() / 1000.0;

    // 5.0V supply corresponds to 1.1V meter output
    voltage = (vcc / 5.0) * 1.1;

    if (voltage > 1.1)
      voltage = 1.1;

    Serial.print("CHECK MODE | Vcc = ");
    Serial.print(vcc, 3);
    Serial.println(" V");
  }

  // ---------------- NORMAL CPM RANGES ----------------
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

    // 1.1V full-scale output
    voltage = 1.1 * ((float)cpm / fullScale);

    // Add your offset above 0.2V
    if (voltage > 0.2)
      voltage += 0.05;

    if (voltage > 1.1)
      voltage = 1.1;

    Serial.print("CPM: ");
    Serial.print(cpm);

    Serial.print(" | Range: 0-");
    Serial.print((unsigned long)fullScale);

    Serial.print(" | Output: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  }

  // PWM output
  int pwmValue = (int)(voltage * 255.0 / 5.0);

  analogWrite(PWM_PIN, pwmValue);

  delay(200);
}