#include "head-tracker.hpp"
#include "const.h"

#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 mpu;

#ifndef JAPPY_HEADTRACKER_GLOBALS
    Quaternion my_q;           // [w, x, y, z]         quaternion container
    VectorFloat my_gravity;    // [x, y, z]            gravity vector
#endif

#define JAPPY_HEADTRACKER_GLOBALS

void HeadTracker::giveFeedback(uint8_t feedbackDuration, uint8_t pauseDuration, uint8_t feedbackCount, bool force = false) {
    if (!feedbackEnabled && !force) {
        return;
    }

    for (int i = 0; i < feedbackCount; i++) {
        digitalWrite(HEAD_TRACKER_FEEDBACK_PIN, LOW);
        delay(feedbackDuration);
        digitalWrite(HEAD_TRACKER_FEEDBACK_PIN, HIGH);
        delay(pauseDuration);
    }
}

void HeadTracker::enableFeedback() {
    feedbackEnabled = true;
}

void HeadTracker::disableFeedback() {
    feedbackEnabled = false;
}

void HeadTracker::begin() {
    pinMode(HEAD_TRACKER_FEEDBACK_PIN, OUTPUT);
    digitalWrite(HEAD_TRACKER_FEEDBACK_PIN, HIGH);

    Wire.begin(HEAD_TRACKER_MPU_SDA_PIN, HEAD_TRACKER_MPU_SCL_PIN);
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

    Serial.begin(115200);

    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    Serial.println(F("Initializing DMP..."));
    uint8_t devStatus = mpu.dmpInitialize();

    mpu.setXAccelOffset(81);
    mpu.setYAccelOffset(1495);
    mpu.setZAccelOffset(1297);
    mpu.setXGyroOffset (52);
    mpu.setYGyroOffset (60);
    mpu.setZGyroOffset (72);

    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        // mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
        giveFeedback(5000, 1000, 3, true);
        while (1) {
            giveFeedback(5000, 1000, 1, false);
        }
    }

    giveFeedback(300, 300, 5, true);
}

void HeadTracker::setOrigin() {
    this->updateOrientation();
    originDegree[0] = this->currentDegree[0];
    originDegree[1] = this->currentDegree[1];
    originDegree[2] = this->currentDegree[2];

    giveFeedback(4000, 1000, 1, true);
}

void HeadTracker::updateOrientation() {
    // read a packet from FIFO
    if (!mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 
        return;
    }

    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&my_q, fifoBuffer);
    mpu.dmpGetGravity(&my_gravity, &my_q);
    float ypr[3];
    mpu.dmpGetYawPitchRoll(ypr, &my_q, &my_gravity);

    currentDegree[0] = ypr[0] * 180/M_PI;
    currentDegree[1] = ypr[1] * 180/M_PI;
    currentDegree[2] = ypr[2] * 180/M_PI;
}

void HeadTracker::tick() {
    this->updateOrientation();

    double maxOffset = 0;
    for (int i = 0; i < 3; i++) {
        double offset = abs(currentDegree[i] - originDegree[i]);
        if (offset > maxOffset) {
            maxOffset = offset;
        }
    }

    bool didMoveTooMuch = maxOffset > HEAD_TRACKER_MAX_ANGLE_OFFSET;

    if (didMoveTooMuch) {
        // vibrate for as many times as the offset is too high
        giveFeedback(100, 200, 1);
    }
}