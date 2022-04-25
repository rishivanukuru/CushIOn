// CushIOn
// Teensy 3.6
// https://rishi-atlas.notion.site/Making-a-CushIOn-fec2e21753d74f1bb63a61bdefdb7036

// Script to measure touch inputs on an 8x8 fabric-velostat grid.

#define numRows 8
#define numCols 8
#define sensorPoints numRows*numCols

// Analog Pins
int rows[] = {1, 2, 3, 4, 5, 6, 7, 8};

// Digital Pins
int cols[] = {3, 4, 5, 6, 7, 8, 9, 10};

// Array to hold input states
int incomingValues[sensorPoints] = {};

// Array to hold key press states
int keyStates[sensorPoints] = {};

// Threshold below which input is sensed (0 - 1023)
int senseThreshold = 1;

// Holder variables for loop operations
int currentKey = 0;
int currentMIDInote = 0;

// MIDI variables

// Offsets to generate notes on the C Major scale
int majorOffset[] = {0, 2, 4, 5, 7, 9, 11, 12};

// Channel for MIDI messages
int MIDIchannel = 1;

// Default key press velocity
int keyPressVelocity = 100;

void setup() {
  
  // set all rows and columns to INPUT (high impedance):
  for (int i = 0; i < numRows; i++) {
    pinMode(rows[i], INPUT_PULLUP);
  }

  for (int i = 0; i < numCols; i++) {
    pinMode(cols[i], INPUT);
  }

  // Initialize the key press array to be all OFF
  for (int i = 0; i < numCols ; i++)
  {
    for (int j = 0; j < numRows ; j++)
    {
      keyStates[i * numRows + j] = 0;
    }
  }

  // Begin serial to send messages to Processing for input visualization
  Serial.begin(38400);

}

void loop() {

  // Sequentially set the voltages of the rows and columns to determine touches
  for (int colCount = 0; colCount < numCols; colCount++)
  {
    pinMode(cols[colCount], OUTPUT); // set as OUTPUT
    digitalWrite(cols[colCount], LOW); // set LOW

    for (int rowCount = 0; rowCount < numRows; rowCount++)
    {
      currentKey = colCount * numRows + rowCount; // 0 to 63
      currentMIDInote = (colCount + 1) * 12 + majorOffset[rowCount]; // index to C Major key

      // If the current input is pressed
      if (analogRead(rows[rowCount]) <= senseThreshold)
      {
        // Set current input to ON
        incomingValues[currentKey] = 1;
        // If the key corresponding to this input was previously OFF
        if (keyStates[currentKey] == 0)
        {
          // Set key state to ON
          keyStates[currentKey] = 1;
          // Send MIDI On
          usbMIDI.sendNoteOn(((colCount + 1) * 12 + majorOffset[rowCount]), keyPressVelocity, MIDIchannel);
        }
      }
      else
      {
        // No input, key value is OFF
        incomingValues[currentKey] = 0;

        // If the key was previously ON
        if (keyStates[currentKey] == 1)
        {
          // Set key to OFF
          keyStates[currentKey] = 0;
          // Send MIDI Off
          usbMIDI.sendNoteOff(((colCount + 1) * 12 + majorOffset[rowCount]), 0, MIDIchannel);
        }
      }
    }

    pinMode(cols[colCount], INPUT); // Set back to INPUT
  }

  // Print the incoming values of the input matrix to serial monitor for Processing
  for (int i = 0; i < sensorPoints; i++)
  {
    Serial.print(incomingValues[i]);
    if (i < sensorPoints - 1)
    {
      Serial.print("\t");
    }
  }
  Serial.println();

  while (usbMIDI.read()) {
    // ignore incoming messages
  }

  // Sample points at intervals of 10 ms
  delay(10);
}
