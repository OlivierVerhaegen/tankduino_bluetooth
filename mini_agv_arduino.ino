/**
 UWB mini AGV
 Name: mini_agv_arduino
 Purpose: Test the functionality of the UWB positioning system.

 @author Olivier Verhaegen, Ben Dupont
 @version 1.0.0
*/

#include <SoftwareSerial.h>

#define L_BCKW 9
#define L_FWD 5
#define R_BCKW 10
#define R_FWD 6

// Setup bluetooth serial communication.
SoftwareSerial EEBlue(A4, A5); // TX/RX

int data = 0;
int directions[2] = {0, 0}; // 0=x, 1=y

void setup() {
  // Initialize Outputs
  pinMode(L_BCKW, OUTPUT);
  pinMode(L_FWD, OUTPUT);
  pinMode(R_BCKW, OUTPUT);
  pinMode(R_FWD, OUTPUT);
  // pinMode(SPD_L, OUTPUT);
  // pinMode(SPD_R, OUTPUT);

  
  Serial.begin(9600);

  // Initialize bluetooth communication.
  /*
   Default communication rate of the bluetooth module = 9600.
   AT command mode communication rate = 38400.
   Default password: 1234 or 0000.
   EN: This pin is used to toggle between Data Mode (free) and AT command mode (ground).
   STATE: This pin is connected to the on-boars LED, can be used for feedback to check if Bluetooth is working properly.
   LED: Indicates the status of the module:
       - Blink once in 2 sec: Module is in AT command mode.
       - Blink twice in 1 sec: Module is in Data mode.
       - Repeated blinking: Waiting for connection in Data mode.
  */
  EEBlue.begin(9600);
}

void loop() {
  // First we recieve X-axis data, then Y-axis data.
  if (EEBlue.available() >= 2) {
    for (int i = 0; i < 2; i++) {
      /*
       Substract velocities by 50 and invert Y axis to get proper values.
       This way the mid-point is (0,0)
       100% FWD is (0,50)
       100% BCKW is (0,-50)
       100% LEFT is (-50,0)
       100% RIGHT is (50,0)
      */
      if (i == 0)
        directions[i] = EEBlue.read() - 50; // X-axis
      if (i == 1)
        directions[i] = (EEBlue.read() - 50) * -1; // Y-axis
    }
  }

  if (directions[0] == 0 && directions[1] == 0) {
    hold();
  } else if (directions[1] > 0) {
    driveForward(directions[0], directions[1]);
  } else if (directions[1] < 0) {
    driveBackward(directions[0], directions[1]);
  }
}

/**
 Turn off the motors of the AGV.
*/
void hold() {
  Serial.println("Hold");
  digitalWrite(R_FWD, LOW);
  digitalWrite(R_BCKW, LOW);
  digitalWrite(L_FWD, LOW);
  digitalWrite(L_BCKW, LOW);
}

/**
 Drive in a forward moving direction.
 Steering is achived by calculating the speed difference between the left and right motor from the @param velocityX.

 @param velocityX The left or right velocity.
 @param velocityY The forward velocity.
*/
void driveForward(int velocityX, int velocityY) {
  int vLeft = velocityY;
  int vRight = velocityY;
  if (velocityX < 0) {
    vLeft += velocityX; // Left needs to go slower, but "+" because velocityX is a negative number.
  }
  if (velocityX > 0) {
    vRight -= velocityX; // Right needs to go slower.
  }

  vLeft = map(vLeft, -50, 50, 0, 255);
  vRight = map(vRight, -50, 50, 0, 255);

  Serial.print("Speed left: ");
  Serial.println(vLeft);
  Serial.println();
  Serial.print("Speed right: ");
  Serial.println(vRight);

  analogWrite(R_FWD, vRight);
  digitalWrite(R_BCKW, LOW);
  analogWrite(L_FWD, vLeft);
  digitalWrite(L_BCKW, LOW);
}

/**
 Drive in a backward moving direction.
 Steering is achived by calculating the speed difference between the left and right motor from the @param velocityX.

 @param velocityX The left or right velocity.
 @param velocityY The forward velocity.
*/
void driveBackward(int velocityX, int velocityY) {
  int vLeft = velocityY * -1;
  int vRight = velocityY * -1;
  if (velocityX < 0) {
    vLeft += velocityX; // Left needs to go slower, but "+" because velocityX is a negative number.
  }
  if (velocityX > 0) {
    vRight -= velocityX; // Right needs to go slower.
  }

  vLeft = map(vLeft, -50, 50, 0, 255);
  vRight = map(vRight, -50, 50, 0, 255);

  Serial.print("Speed left: ");
  Serial.println(vLeft);
  Serial.println();
  Serial.print("Speed right: ");
  Serial.println(vRight);

  digitalWrite(R_FWD, LOW);
  analogWrite(R_BCKW, vRight);
  digitalWrite(L_FWD, LOW);
  analogWrite(L_BCKW, vLeft);
}
