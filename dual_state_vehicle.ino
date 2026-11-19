// --- Motor Direction Control Pins (Wired to L298N) ---
int leftForward = 4;  // IN1
int leftBackward = 5; // IN2
int rightForward = 6; // IN3
int rightBackward = 7;// IN4

// --- New Motor Speed Control Pins (Jumpers removed from L298N!) ---
int enableA = 11;      // ENA (Left Motors Speed - must be a PWM pin)
int enableB = 3;      // ENB (Right Motors Speed - must be a PWM pin)

// --- Ultrasonic Sensor Pins ---
int trigPin = 9;
int echoPin = 10;

// --- State Variables for System Control ---
bool emergencyStop = true; // Starts the robot in a locked safety state 
char manualCommand = ' '; 
int motorSpeed = 110; // Default cruising speed (Scale: 0 to 255)

void setup() {
  pinMode(leftForward, OUTPUT);
  pinMode(leftBackward, OUTPUT);
  pinMode(rightForward, OUTPUT);
  pinMode(rightBackward, OUTPUT);
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600); 
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// --- Dynamic Directional Actuation Functions ---
void applySpeed() {
  analogWrite(enableA, motorSpeed); // Sends PWM speed signal to Left side
  analogWrite(enableB, motorSpeed); // Sends PWM speed signal to Right side
}

void stopMotors() {
  digitalWrite(leftForward, LOW);
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(rightBackward, LOW);
}

void goForward() {
  applySpeed();
  digitalWrite(leftForward, HIGH);
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, HIGH);
  digitalWrite(rightBackward, LOW);
}

void goBackward() {
  applySpeed();
  digitalWrite(leftForward, LOW);
  digitalWrite(leftBackward, HIGH);
  digitalWrite(rightForward, LOW);
  digitalWrite(rightBackward, HIGH);
}

void turnRight() {
  applySpeed();
  digitalWrite(leftForward, HIGH);
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(rightBackward, HIGH);
}

void turnLeft() {
  applySpeed();
  digitalWrite(leftForward, LOW);
  digitalWrite(leftBackward, HIGH);
  digitalWrite(rightForward, HIGH);
  digitalWrite(rightBackward, LOW);
}

void loop() {
  // 1. BLUETOOTH SERIAL COMMAND PARSER
  if (Serial.available() > 0) {
    char incomingChar = Serial.read();
    
    // Safety Gates
    if (incomingChar == 'S' || incomingChar == 's') {
      emergencyStop = true;
      manualCommand = ' ';
    } 
    else if (incomingChar == 'G' || incomingChar == 'g') {
      emergencyStop = false;
    }
    
    // Dynamic Speed Modifiers
    else if (incomingChar == 'F' || incomingChar == 'f') { 
      motorSpeed = 255; // Boost to absolute maximum velocity
    }
    else if (incomingChar == 'W' || incomingChar == 'w') { // 'W' for Slow / Standard Walk
      motorSpeed = 130; // Reduce voltage for precise low-speed maneuvering
    }
    
    // Macro-Action Triggers (90-Degree Precision Sweeps)
    else if (incomingChar == 'C' || incomingChar == 'c') { // Clockwise 90
      turnRight();
      delay(650); // Calibrate this millisecond value for your specific flooring!
      stopMotors();
    }
    else if (incomingChar == 'A' || incomingChar == 'a') { // Anticlockwise 90
      turnLeft();
      delay(650); // Calibrate this millisecond value for your specific flooring!
      stopMotors();
    }
    
    // Continuous Steering Commands
    else if (incomingChar == 'L' || incomingChar == 'l' || 
             incomingChar == 'R' || incomingChar == 'r' || 
             incomingChar == 'B' || incomingChar == 'b') {
      manualCommand = incomingChar;
    }
  }

  // 2. RUNTIME EXECUTION MATRIX
  if (emergencyStop == true) {
    stopMotors();
  } 
  else {
    if (manualCommand == 'B' || manualCommand == 'b') {
      goBackward();
    } 
    else if (manualCommand == 'L' || manualCommand == 'l') {
      turnLeft();
      delay(300);
      manualCommand = ' '; 
    } 
    else if (manualCommand == 'R' || manualCommand == 'r') {
      turnRight();
      delay(300);
      manualCommand = ' '; 
    } 
    else {
      // --- Default State: Autonomous Obstacle Avoidance ---
      long distance = getDistance();
      
      // Increased safety buffer to 35cm to give the chassis room to swing
      if (distance > 0 && distance < 35) {
        
        // Step A: Slam the brakes instantly
        stopMotors();
        delay(150); 
        
        // Step B: Back up a tiny bit to pull away from the object
        goBackward();
        delay(300); 
        
        // Step C: Spin right into safety
        turnRight(); 
        delay(400); // Gives it a clean 45-to-90 degree escape angle
        
        // Step D: Reset and clear the motors before moving forward again
        stopMotors();
        delay(100);
      } 
      else {
        // Path is completely clear, keep cruising
        goForward();
      }
    }
  }
  delay(30); // Lowered from 50 to 30 for faster sensor reaction times
}