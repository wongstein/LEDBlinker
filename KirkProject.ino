/*
  Kirk Project Pressure Sensor Code
*/

// the setup routine runs once when you press reset:
const int numReadings = 200;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long total = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;

long ledtimethresh = 1;
long leddutythresh = 10000000;
long ledtime = 0;
long ledduty = 0;
int templed = 0;

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  pinMode(10, OUTPUT);
}

void loop() {
  ledtime = ledtime + 1;
  ledduty = ledduty + 1;
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);        // delay in between reads for stability

  if (ledtime > ledtimethresh){
    ledtime = 0;
    if (average > 100){     // Switch statement should be here
      leddutythresh = 3000;
      digitalWrite(10,HIGH);
    }
    else{
      leddutythresh = 200000;
      digitalWrite(10,LOW);
    }
  }

/*    // This is where the LED turns on/off based on leddutythresh, which is set in the switch statement above
  if (ledduty > leddutythresh){
    ledduty = 0;
    if (templed == 1){
      templed = 0;
    }
    else {
      templed = 1;
    }
    digitalWrite(10,templed);
  }*/
 
}
