int clock = 8; // Clock pin for the shift register
int data = 13; // Data pin for the shift register
int latch = 12; // Latch pin for the shift register
int digits[] = {6, 9, 10, 11}; // Output pins for the V+ of each segment of the display
int buttons[] = {7, 4, 2}; // Input pins for the input buttons
int speaker = 5; //; Speaker pin
unsigned long lastTime; // used for incrementing minutes
unsigned long lastRead; // used for button cooldowns
 
int numbers[] = { // The different values that should be output to the shift register to get numbers, in order from 0 to 9.
  245,
  68,
  115,
  87,
  198,
  151,
  183,
  69, //nice
  247,
  199
};
 
void setup() {                
  pinMode(clock, OUTPUT); // clock
  pinMode(data, OUTPUT); // data
  pinMode(latch, OUTPUT); // latch
  pinMode(speaker, OUTPUT); // speaker
  pinMode(digits[0], OUTPUT); // led
  digitalWrite(digits[0], LOW); // Ensure LEDs are off to start
  pinMode(digits[1], OUTPUT); // led
  digitalWrite(digits[1], LOW); // Ensure LEDs are off to start
  pinMode(digits[2], OUTPUT); // led
  digitalWrite(digits[2], LOW); // Ensure LEDs are off to start
  pinMode(digits[3], OUTPUT); // led
  digitalWrite(digits[3], LOW); // Ensure LEDs are off to start
  pinMode(buttons[0], INPUT); // button
  pinMode(buttons[1], INPUT); // button
  pinMode(buttons[2], INPUT); // button
  lastTime = millis(); // Set initial time to start clock
  lastRead = millis(); // Initial time
  //Serial.begin(9600); // debugging
}

void writeShiftReg(int number) {
  digitalWrite(latch, LOW);
  shiftOut(data, clock, MSBFIRST, numbers[number]); // Send the value of the number to the shift register / screen
  digitalWrite(latch, HIGH);
}

void writeToScreen(int number) {
  int holdMe = 0; // A temporary number used to track current part of the display
  while (number > 0) {
    int digit = number % 10; // Get the digit at current position
    number /= 10; // Remove the digit at current position
    writeShiftReg(digit); // Prep to display number to screen
    digitalWrite(digits[holdMe], HIGH); // Enable LEDs for screen section
    delay(2);
    digitalWrite(digits[holdMe], LOW); // Disable LEDs so the next number can be shown
    holdMe++; // Increment to next display part
  }
}

void loop() {
  static int disTime = 1; // Current time (displayed time is stored as an integer, divided by 60 later to print properly to screen)
  static int alarmTime = 0; // When the alarm should go off
  static boolean buzz = true; // Buzz? buzz buzz. (Self explanatory)
  if (digitalRead(buttons[2]) == HIGH) { // Used to show the currently set alarm time rather than the time when changing alarm
    writeToScreen(((alarmTime / 60) * 100) + (alarmTime % 60) + 10000); // Show time
  } else {
    writeToScreen(((disTime / 60) * 100) + (disTime % 60) + 10000); // Show time
  }
  delay(5);
  
  if (millis() - lastRead > 100) { // Cooldown for button interaction
    if (digitalRead(buttons[0]) == HIGH) { // Button to modify time on?
      if (digitalRead(buttons[1]) == HIGH) { // Modify button clicked?
        disTime += 1; // Increment time
        lastRead = millis(); // Set button cooldown
      }
    } else if (digitalRead(buttons[2]) == HIGH) { // Modify alarm time
      if (digitalRead(buttons[1]) == HIGH) { // Modify on
        alarmTime += 1; // Increment alarm time
        lastRead = millis(); // Cooldown
      }
    } else if (digitalRead(buttons[1]) == HIGH) { // Snooze button
      noTone(speaker); // Stop playing noise on speaker
      buzz = false; // Cancel the buzz for now
      lastRead = millis(); // Button cooldown
    }
  }
  if (millis() - lastTime >= 60000) { // Has it been 60 seconds since last read?
    lastTime = millis(); // Set time for time checking
    disTime += 1; // Increment time
    buzz = true; // Allow buzzing
  }
  if (disTime == alarmTime) { // Buzz if it is alarm time
    if (buzz) { // Not snoozed
      tone(speaker, 40); // beep
    }
  }
  if (disTime >= 1440) { // Time rollover at days end
    disTime = 0; // time 0
  }
  /* Serial.print(digitalRead(buttons[0])); //Debugging, shows each buttons state and what times the clock thinks it has
  Serial.print(digitalRead(buttons[1]));
  Serial.print(digitalRead(buttons[2]));
  Serial.print(" ");
  Serial.print(alarmTime);
  Serial.print(" ");
  Serial.print(disTime);
  Serial.print(" ");
  Serial.println((millis() - lastTime) / 1000); */
}
