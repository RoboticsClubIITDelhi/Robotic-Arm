#include <SBUS.h>
#include <SoftwareSerial.h>

const int BASE_STEPPER_PIN = 34;
const int X_STEPPER_PIN = 34;
const int Y_STEPPER_PIN = 34;

const int X_SERVO_PIN = 34;
const int X_SERVO = 34; // 0 for SC15 1 for MG995 
const int Y_SERVO_PIN = 34;
const int Y_SERVO = 34; // 0 for SC15 1 for MG995
const int END_SERVO_PIN = 34;

const int LIM_SWITCH_BASE_PIN = 35;
const bool LIM_SWITCH_BASE_ACT = true;
bool LIM_SWITCH_BASE_VAL = false;
String receivedData = ""; // To store received data
char basemotor;
int motorsteps;
const int op_mode = 3; 
//op_mode - mode  - x_motor - y_motor
// 0       - setup - sc15    - stepper
// 1       - setup - sc15    - sc15
// 2       - setup - mg995   - mg995
// 3       - setup - stepper - stepper

// 4       - setup - sc15    - stepper
// 5       - setup - sc15    - sc15
// 6       - setup - mg995   - mg995
// 7       - setup - stepper - stepper
#include <AccelStepper.h>

// Define pins for STEP and DIR
#define STEP_PIN 33
#define DIR_PIN 32

// Initialize stepper in DRIVER mode (step/dir interface)
AccelStepper stepper1(1, STEP_PIN, DIR_PIN);

// Constants
const int stepsPerRevolution = 3200; // Update this based on your motor and driver settings
const int stepsPerDegree = stepsPerRevolution / 360; // Steps per degree

int stepper_val;

#define S_RXD 16
#define S_TXD 17

#include <SCServo.h>

SCSCL sc;
int scx_pos;
int scy_pos;

#include <ESP32Servo.h>

Servo end;
int end_pos;


void move_steps(char step_dir, int steps) {
    if (step_dir == 'b') {
        stepper1.move(stepsPerDegree * ((float)steps/5));
        stepper_val += steps;
    } else if (step_dir == 'x') {
        scx_pos = max(min(scx_pos+steps, 1023), 0);
        sc.WritePosEx(0x02, scx_pos, 0, 5);
    } else if (step_dir == 'y') {
        scy_pos = max(min(scy_pos+steps, 1023), 0);
        sc.WritePosEx(0x01, scy_pos, 0, 5);
    } else if (step_dir == 'e') {
        end_pos = steps;
        end.write(steps);
    }
    Serial.print("Stepper: ");
    Serial.print(stepper_val);
    Serial.print(" SCX: ");
    Serial.print(scx_pos);
    Serial.print(" SCY: ");
    Serial.print(scy_pos);
    Serial.print(" End: ");
    Serial.print(end_pos);
    read_lim_switch();
}

void setup() {
    end.attach(25);
    Serial.begin(115200);
    Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
    sc.pSerial = &Serial1;    
    // Serial1.begin(9600);
    stepper1.setMaxSpeed(200);   // Steps per second
    stepper1.setAcceleration(100); // Steps per second squa 
    if (LIM_SWITCH_BASE_ACT) {pinMode(LIM_SWITCH_BASE_PIN, INPUT_PULLDOWN);}
    home_base();
    home_xy();
}

void loop() {
    if (Serial.available() > 0) {
        receivedData = Serial.readStringUntil('\n');
        int firstSpace = receivedData.indexOf(' ');
        int secondSpace = receivedData.indexOf(' ', firstSpace + 1);
        basemotor = receivedData.substring(0, firstSpace)[0];
        motorsteps = receivedData.substring(firstSpace + 1, secondSpace).toInt();
        move_steps(basemotor, motorsteps);
    }
    if (stepper1.distanceToGo() != 0) {
        stepper1.run();
    }
}

void read_lim_switch() {
    if (LIM_SWITCH_BASE_ACT) {
        int x = digitalRead(LIM_SWITCH_BASE_PIN);
        LIM_SWITCH_BASE_VAL = (bool)x;
        Serial.println(LIM_SWITCH_BASE_VAL);
    }
}

void home_base() {
    // go to home
    while (!LIM_SWITCH_BASE_VAL) {
        move_steps('b', -2);
        if (stepper1.distanceToGo() != 0) {
            stepper1.run();
        }
    }
    stepper_val = 0;
}

void home_xy() {
    sc.WritePosEx(0x01, 600, 0, 5);
    sc.WritePosEx(0x02, 600, 0, 5);
    scx_pos = 600;
    scy_pos = 600;
}