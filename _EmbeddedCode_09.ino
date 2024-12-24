#include <Arduino_LSM9DS1.h>
#include <math.h>

const float accel_threshold = 1.5;
const float gyro_threshold = 200;
const float free_fall_threshold = 0.5;
const float impact_threshold = 2.5;
const int post_fall_orientation_time = 3000;

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;

void setup() {
  Serial.begin(9600);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.println("IMU initialized!");
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(accelX, accelY, accelZ);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gyroX, gyroY, gyroZ);
  }

  float accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  float gyroMagnitude = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);

  if (accelMagnitude > accel_threshold || gyroMagnitude > gyro_threshold) {
    Serial.println("Pre-fall detected!");

    if (accelMagnitude < free_fall_threshold) {
      Serial.println("Free fall detected!");
      delay(1000);

      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(accelX, accelY, accelZ);
      }

      accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

      if (accelMagnitude > impact_threshold) {
        Serial.println("Impact detected!");
        unsigned long startTime = millis();
        while (millis() - startTime < post_fall_orientation_time) {
          if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(accelX, accelY, accelZ);
          }

          accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

          if (accelMagnitude > impact_threshold) {
            Serial.println("Fall confirmed!");
            break;
          }
        }
      }
    }
  }

  delay(100);
}
