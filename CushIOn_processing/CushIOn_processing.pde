/*
Code for the CushIOn interface
 Reads X analog inputs and visualizes them by drawing a grid
 */

// Serial communication library
import processing.serial.*;

// Background Image for overlays (not required)
PImage bg;

// The serial port
Serial myPort; 

// Grid Details
int rows = 8;
int cols = 8;
int maxNumberOfSensors = rows*cols;

// Global variable for storing mapped sensor values
float[] sensorValue = new float[maxNumberOfSensors]; 
// Array of previous values
float[] previousValue = new float[maxNumberOfSensors]; 

// Input rectangle variables
int rectSize = 0;
int rectY;

void setup () {
  // Set up the window to screen size
  size(1800, 1800);
  
  // Define rectangle size based on width
  rectSize = width/rows;

  // List all the available serial ports
  println(Serial.list()); 
  
  // Set the number of the Teensy serial port
  String portName = Serial.list()[2]; 
  
  // Initialize Port
  myPort = new Serial(this, portName, 9600);
  myPort.clear();
  // Don’t generate a serialEvent() until you get a newline (\n) byte
  myPort.bufferUntil('\n'); 
  
  // UNCOMMENT IF USING OVERLAYS
  // bg = loadImage("numpadbg.png");
  // background(bg); // set inital background
  
  // Turn on antialiasing
  smooth(); 
  
  // Setting rectangle draw mode  
  rectMode(CORNER);
}

void draw () {
  
  // UNCOMMENT IF USING OVERLAYS
  // background(bg); // set overlay background

  // COMMENT IF USING OVERLAYS
  // Set white background
  background(255);
  
  // Read through sensor color values
  for (int i = 0; i < maxNumberOfSensors; i++) {
    
    // If white, set to transparent
    if (sensorValue[i] == 255)
    {
      fill(sensorValue[i], 0);
    } 
    // Else, set black
    else
    {
      fill(sensorValue[i]);
    }
    // Draw rectangle at the correct position for index (mapping 0 to 63 to an 8x8 grid)
    rect(  rectSize * (i%rows), rectY, rectSize, rectSize); //top left
    if ((i+1) % rows == 0) rectY += rectSize;
  }
  rectY=0;
}

// Function to receive and process serial messages from the Teensy
void serialEvent (Serial myPort) {
  // Get the ASCII string
  String inString = myPort.readStringUntil('\n'); 
  // If the string is not empty
  if (inString != null) { 
    // Trim off any whitespace
    inString = trim(inString); 
    // Convert to an array of ints
    int incomingValues[] = int(split(inString, '\t')); 

    // As long as the serial message is in the right format
    if (incomingValues.length <= maxNumberOfSensors && incomingValues.length > 0) {
      for (int i = 0; i < incomingValues.length; i++) {
        // Map the incoming values (0 or 1) to an appropriate color (0 or 255):
        if (incomingValues[i] == 0)
        {
          sensorValue[i] = 255;
        } else
        {
          sensorValue[i] = 0;
        }
        // UNCOMMENT IF IN DEBUG MODE
        // println(sensorValue[i]); // Print value in Processing Console
      }
    }
  }
}
