//JLC
// 2020/11/25 
// Arduino UNO R3 
// Weighted average data aquisition & storage program for the MPU6050 accelerometer sensor.
// Uses an SD card for data storage.

#include "Wire.h"
#include <MPU6050_light.h>
#include <SD.h>

//Set up routine for the MPU6050
MPU6050 mpu(Wire);

void setup() {
  //Start screen serial monitor
  Serial.begin(9600);
//    while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  Serial.println("New process");
  
  Wire.begin();
  mpu.begin();
  // The library sets up the accelerometer as 2g and 250 deg/s
  // define gyro and acceleroconfig (from 0 to 3)
  byte status = mpu.begin (0,0) ;
  mpu.calcOffsets(); //compute the gyroscope and accelerometer offsets.
  // if initialsation error stop
  while(status != 0) {}
  // define which offset to compute (gryo or accelerometer)
  mpu.calcOffsets (true, true);
  // define a custom complementary filter (from 0.0 to 1.0)
  mpu.setFilterGyroCoef(0.98);
  
/*Connections
    SD card Module - Arduino UNO
              MOSI - Pin 11
              MISO - Pin 12
              CLK  - Pin 13
              CS   - Pin 10
*/
const int chipSelect = 10;
while (!Serial) 
  {
    ;
  }

 // Serial.print("\nInitializing SD card...");

  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Initialization failed. Things to check:");
    Serial.println("* Is a card inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* Did you change the chipSelect pin to match your shield or module?");
    while (1);
  } 
  else 
  {
    Serial.println("A card is present and wiring is correct.");
  }

//Write a header on the SD card
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  dataFile.println("Time,  Ax,  Ay,  Az,  GyX,  GyY,  GyZ");
  dataFile.close();
}

  float accel[] = {0.0, 0.0, 0.0};
  float gyro[] = {0.0, 0.0, 0.0};
  float time, N = 5.0; //Time is the number of seconds for a time stamp. N - number of data samples for the exponential moving average
  float k = 2.0/(N+1); //Factor for wieghting of the time series data
  float kNeg = 1.0-k; 

void loop() {
  // updates the data to get consistent angle data from the sensor
  mpu.update();

  // capture data in a small array
  time = float(millis()) / 1000.0;
  accel[0] = k * mpu.getAccX() + kNeg * accel[0];  
  accel[1] = k * mpu.getAccY() + kNeg * accel[1];
  accel[2] = k * mpu.getAccZ() + kNeg * accel[2];

  gyro[0] = k * mpu.getGyroX() + kNeg * gyro[0];
  gyro[1] = k * mpu.getGyroY() + kNeg * gyro[1];
  gyro[2] = k * mpu.getGyroZ() + kNeg * gyro[2];


// Serial terminal output for the times needed. Comment out for when not needed. 
  Serial.print("Time: "); Serial.print(time, 2);
  
  Serial.print(" -- Accelerometer: ");
  Serial.print(" X = ");     Serial.print(accel[0],3);
  Serial.print(" | Y = ");  Serial.print(accel[1],3);
  Serial.print(" | Z = ");  Serial.print(accel[2],3); 

  Serial.print(" -- Gyroscope: ");
  Serial.print("X = "); Serial.print(gyro[0],2);
  Serial.print(" | Y = "); Serial.print(gyro[1],2);
  Serial.print(" | Z = "); Serial.print(gyro[2],2);
  Serial.println(" ");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(time, DEC); 
    dataFile.print(", ");
    dataFile.print(accel[0],3);
    dataFile.print(", ");
    dataFile.print(accel[1],3);
    dataFile.print(", ");
    dataFile.print(accel[2],3);
//    dataFile.print(", ");
//    dataFile.print(gyro[0],3);
//    dataFile.print(", ");
//    dataFile.print(gyro[1],3);
//    dataFile.print(", ");
//    dataFile.print(gyro[2],3);
//    dataFile.print(", ");
//    dataFile.print(Tmp/340+36.54,3);
    dataFile.println();
    dataFile.close();
 }

  // if the file isn't open, pop up an error:
  else {
//    Serial.println("error opening datalog.csv");
  }
}
