// WIFI_AP settings.
const char* AP_SSID = "ESP32_DEV";
const char* AP_PWD  = "12345678";

// WIFI_STA settings.
const char* STA_SSID = "GalaxyA33";
const char* STA_PWD  = "12345678";

// the MAC address of the device you want to ctrl.
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x93, 0x5F, 0xA8};
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


typedef struct struct_message {
  int ID_send;
  int POS_send;
  int Spd_send;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// set the default role here.
// 0 as normal mode.
// 1 as leader, ctrl other device via ESP-NOW.
// 2 as follower, can be controled via ESP-NOW.
#define DEFAULT_ROLE 0

// set the default wifi mode here.
// 1 as [AP] mode, it will not connect other wifi.
// 2 as [STA] mode, it will connect to know wifi.
#define DEFAULT_WIFI_MODE 1

// the uart used to control servos.
// GPIO 18 - S_RXD, GPIO 19 - S_TXD, as default.
#define S_RXD 18
#define S_TXD 19

// the IIC used to control OLED screen.
// GPIO 21 - S_SDA, GPIO 22 - S_SCL, as default.
#define S_SCL 22
#define S_SDA 21

// the GPIO used to control RGB LEDs.
// GPIO 23, as default.
#define RGB_LED   23
#define NUMPIXELS 10

// set the max ID.
int MAX_ID = 20;

// modeSelected.
// set the SERIAL_FORWARDING as true to control the servos with USB.
bool SERIAL_FORWARDING = false;

// OLED Screen Dispaly.
// Row1: MAC address.
// Row2: VCC --- IP address.
// Row3: MODE:Leader/Follower  [AP]/[STA][RSSI]
//       DEFAULT_ROLE: 1-Leader(L)/ 2-Follower(F).
//       DEFAULT_WIFI_MODE: 1-[AP]/ 2-[STA][RSSI] / 3-[TRY:SSID].
//       (no matter what wifi mode you select, you can always ctrl it via ESP-NOW.)
// Row4: the position of servo 1, 2 and 3.
String MAC_ADDRESS;
IPAddress IP_ADDRESS;
uint8_t   SERVO_NUMBER;
uint8_t   DEV_ROLE;
uint8_t   WIFI_MODE;
int    WIFI_RSSI;
int pamphlet = 2;

// set the interval of the threading.
#define threadingInterval 600
#define clientInterval    10

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include "RGB_CTRL.h"
#include "STSCTRL.h"
#include "CONNECT.h"
#include "BOARD_DEV.h"
#include <bits/stdc++.h>
using namespace std;

#undef PI
const double PI = 3.141;
const double TOL = 2.5;

double x0(double theta) { return 170 * cos(theta); }
double y0(double theta) { return 170 * sin(theta); }

double x1_center(double phi) { return -20 - 50 * cos(phi); }
double y1_center(double phi) { return 50 * sin(phi); }

double radToDegWrapped(double rad) {
    double deg = rad * 180.0 / PI;
    while (deg > 180.0) deg -= 360.0;
    while (deg <= -180.0) deg += 360.0;
    return deg;
}

double dist(double theta, double phi) {
    double dx = x1_center(phi) - x0(theta);
    double dy = y1_center(phi) - y0(theta);
    return std::hypot(dx, dy);
}

std::vector<std::pair<double, double>> intersections(double theta, double phi) {
    std::vector<std::pair<double, double>> points;

    double x0c = x0(theta), y0c = y0(theta);
    double x1c = x1_center(phi), y1c = y1_center(phi);
    double dx = x1c - x0c, dy = y1c - y0c;
    double d = std::hypot(dx, dy);

    if (d > 220 || d < 120) return points;

    double a = (2500 - 28900 + d*d) / (2*d);
    double h = sqrt(2500 - a*a);

    double xm = x0c + a * dx / d;
    double ym = y0c + a * dy / d;

    double rx = -dy * (h / d);
    double ry = dx * (h / d);

    points.emplace_back(xm + rx, ym + ry);
    points.emplace_back(xm - rx, ym - ry);

    return points;
}

bool satisfies_constraint(double x_known, double y_known, double theta, std::pair<double, double> pt) {
    double x1 = pt.first, y1 = pt.second;

    double lhs_x = x_known - x0(theta);
    double rhs_x = 0.8 * (x_known - x1);

    double lhs_y = y_known - y0(theta);
    double rhs_y = 0.8 * (y_known - y1);

    return std::abs(lhs_x - rhs_x) < TOL && std::abs(lhs_y - rhs_y) < TOL;
}

std::vector<std::pair<double, double>> solveInverseKinematics(double x_known, double y_known) {
    std::vector<std::pair<double, double>> results;

    for (double theta = 0; theta < 2 * PI; theta += 0.1) {
        for (double phi = 0; phi < 2 * PI; phi += 0.1) {
            auto pts = intersections(theta, phi);
            for (auto& pt : pts) {
                if (satisfies_constraint(x_known, y_known, theta, pt)) {
                    double theta_deg = radToDegWrapped(theta);
                    double phi_deg = radToDegWrapped(phi);
                    if (theta_deg >= -90 && theta_deg <= 90 &&
                        phi_deg >= -30 && phi_deg <= 90) {
                        results.emplace_back(theta, phi);
                    }
                }
            }
        }
    }

    return results;
}

std::vector<std::pair<double, double>> adaptiveSmoothMove(double x_known, double y_known) {
    std::vector<std::pair<double, double>> results;

    for (double theta = 0; theta < 2 * PI; theta += 0.01) {
        for (double phi = 0; phi < 2 * PI; phi += 0.01) {
            auto pts = intersections(theta, phi);
            for (auto& pt : pts) {
                if (satisfies_constraint(x_known, y_known, theta, pt)) {
                    double theta_deg = radToDegWrapped(theta);
                    double phi_deg = radToDegWrapped(phi);

                    if (theta_deg >= -90 && theta_deg <= 90 &&
                        phi_deg >= -30 && phi_deg <= 90) {
                        results.emplace_back(theta, phi);
                    }
                }
            }
        }
    }

    return results;
}


uint8_t ID[2];
uint16_t Position[2];
uint16_t Speed[2];


void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  espNowInit();

  getMAC();
  
  boardDevInit();

  RGBcolor(0, 64, 255);

  servoInit();

  wifiInit();

  webServerSetup();

  RGBoff();

  delay(1000);
  pingAll(true);

  threadInit();

}

void smoothMove3Servos(uint8_t servo1ID, uint8_t servo2ID, uint8_t servo3ID, int16_t target1, int16_t target2, int16_t target3, int steps, int stepDelay) {
    
    // Get current positions
    int16_t current1 = posRead[servo1ID];
    int16_t current2 = posRead[servo2ID]; 
    int16_t current3 = posRead[servo3ID];
    
    // Calculate differences
    int16_t diff1 = target1 - current1;
    int16_t diff2 = target2 - current2;
    int16_t diff3 = target3 - current3;
    
    // Calculate step increments
    float step1 = (float)diff1 / steps;
    float step2 = (float)diff2 / steps;
    float step3 = (float)diff3 / steps;
    
    // Arrays for SyncWritePos
    uint8_t servoIDs[3] = {servo1ID, servo2ID, servo3ID};
    uint16_t positions[3];
    uint16_t speed[3];
    speed[0] = 1000;
    speed[1] = 1000;
    speed[2] = 1000; 
    
    // Execute smooth movement
    for(int i = 1; i <= steps; i++) {
        // Calculate intermediate positions
        positions[0] = current1 + (u16)(diff1*i / steps);
        positions[1] = current2 + (u16)(diff2*i / steps);
        positions[2] = current3 + (u16)(diff3*i / steps);
        
        // Send synchronized position command
        st.SyncWritePos(servoIDs, 3, positions, 0, speed);
        
        // Wait before next step
        delay(stepDelay);
        
        // Optional: Update position readings for feedback
        if(i % 5 == 0) { // Update every 5 steps to avoid too frequent reads
            getFeedBack(servo1ID);
            getFeedBack(servo2ID);
            getFeedBack(servo3ID);
        }
    }
    
    // Final position to ensure accuracy
    positions[0] = target1;
    positions[1] = target2;
    positions[2] = target3;
    st.SyncWritePos(servoIDs, 3, positions, 0, speed);
    
    // Update final positions
    delay(100);
    getFeedBack(servo1ID);
    getFeedBack(servo2ID);
    getFeedBack(servo3ID);
}

void adaptiveSmoothMove(uint8_t servo1ID, uint8_t servo2ID, uint8_t servo3ID, int16_t target1, int16_t target2, int16_t target3) {
    
    // Get current positions
    int16_t current1 = posRead[servo1ID];
    int16_t current2 = posRead[servo2ID];
    int16_t current3 = posRead[servo3ID];
    
    // Calculate absolute differences
    int16_t diff1 = abs(target1 - current1);
    int16_t diff2 = abs(target2 - current2);
    int16_t diff3 = abs(target3 - current3);
    
    // Find the largest movement
    int16_t maxDiff = max(diff1, max(diff2, diff3));
    
    // Calculate adaptive steps (more steps for larger movements)
    int adaptiveSteps = 5+ maxDiff/45;    
    // Calculate adaptive delay (slower for larger movements)
    int adaptiveDelay = 20;
    
    // Call smooth move with adaptive parameters
    smoothMove3Servos(servo1ID, servo2ID, servo3ID, target1, target2, target3, adaptiveSteps, adaptiveDelay);
}


void loop() {
  delay(20000);
}


// > > > > > > > > > DOC < < < < < < < < <
// === Develop Board Ctrl ===
// get the MAC address and save it in MAC_ADDRESS;
// getMAC();

// Init GPIO.
// pinMode(PIN_NUM, OUTPUT);
// pinMode(PIN_NUM, INPUT_PULLUP);

// set the level of GPIO.
// digitalWrite(PIN_NUM, LOW);
// digitalWrite(PIN_NUM, HIGH);

// PWM output(GPIO).
// int freq = 50;
// resolution = 12;
// ledcSetup(PWM_NUM, frep, resolution);
// ledcAttachPin(PIN_NUM, PWM_NUM);
// ledcWrite(PWM_NUM, PWM);


// === Servo Ctrl ===
// GPIO 18 as RX, GPIO 19 as TX, init the serial and the servos.
// servoInit();

// set the position as middle pos.
// setMiddle(servoID);
// st.WritePosEx(ID, position, speed, acc);



// === Devices Ctrl ===
// ctrl the RGB.
// 0 < (R, G, B) <= 255
// setSingleLED(LED_ID, matrix.Color(R, G, B));

// init the OLED screen, RGB_LED.
// boardDevInit();

// dispaly the newest information on the screen.
// screenUpdate();
void dance() {
  adaptiveSmoothMove(1,2,3, static_cast<int16_t>(885),static_cast<int16_t>(845),static_cast<int16_t>(385));
  delay(500);
  adaptiveSmoothMove(1,2,3, static_cast<int16_t>(480),static_cast<int16_t>(845),static_cast<int16_t>(385));
  delay(500);
  st.WritePosEx(4,650,500,0);
  delay(1000);
  adaptiveSmoothMove(1,2,3, static_cast<int16_t>(480),static_cast<int16_t>(920),static_cast<int16_t>(500));
  delay(500);
  st.WritePosEx(4,150,500,0);
  delay(1000);
  adaptiveSmoothMove(1,2,3, static_cast<int16_t>(480),static_cast<int16_t>(870),static_cast<int16_t>(800));
  delay(500);
  st.WritePosEx(1,885,700,0);
  delay(500);
}
void ik(int cmdI, int cmdA,int cmdB){
    int theta = cmdA;
    int r = cmdI;
    int z = cmdB;
    vector<pair<double, double>> angles = solveInverseKinematics(r, z);
    if (angles.empty()) {} 
    else {
      double theta_1 = angles[0].first;
      double theta_2 = angles[0].second;
      int off_1 = 462;
      int off_2 = 840;
      int off_3 = 710;
      int s1 = off_1 - theta*1024/180;
      int s2 = off_2 - (theta_1-90)*1024/180;
      int s3 = off_3 + (theta_2)*1024/180;
      adaptiveSmoothMove(1,2,3, static_cast<int16_t>(s1),static_cast<int16_t>(s2),static_cast<int16_t>(s3));
    }
}