#include "Includes.h"
#define GYRO_ADDR    0x68
// 0x68
// #define ACCL_ADDR

#define PWR_MGMT_1        0x6B
#define PWR_MGMT_1_DATA   0x00
//turn on

#define GYRO_CONFIG       0x1B
#define GYRO_CONFIG_DATA  0x08
//DPS setting

#define ACC_CONFIG       0x1C
#define ACC_CONFIG_DATA  0x10

#define OUT_ALL         59
#define OUT_X_H         0x43
#define OUT_X_L         0x44
#define OUT_Y_H         0x45
#define OUT_Y_L         0x46
#define OUT_Z_H         0x47
#define OUT_Z_L         0x48

#define TEMP_H          65
#define TEMP_L          66

#define ACC_OUT_X_H     59
#define ACC_OUT_X_L     60
#define ACC_OUT_Y_H     61
#define ACC_OUT_Y_L     62
#define ACC_OUT_Z_H     63
#define ACC_OUT_Z_L     64

typedef union{
  i8 arr8[14];
  i16 arr16[7];
  struct{
    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }gyroX;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }gyroY;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }gyroZ;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }temp;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }accX;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }accY;

    union{
      i16 full;
      struct{
        i8 high;
        i8 low;
      };
    }accZ;

  };
}MPU_Data;

  typedef union{
  float arrf[3];
  struct{
  float pitch;
  float roll;
  float yaw;
  };
  }rotation_axis;

MPU_Data calibrationData;
long acc_total_vector;

static inline
void gyroWrite8(const u8 reg, const u8 data)
{
  Wire.beginTransmission(GYRO_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void gyroReadXYZ(MPU_Data *data)
{
  Wire.beginTransmission(GYRO_ADDR);
  Wire.write(OUT_ALL);
  Wire.endTransmission();
  Wire.requestFrom(GYRO_ADDR, 14);
  while(Wire.available()<14);

  data->arr8[9] = Wire.read();
  data->arr8[8] = Wire.read();
  data->arr8[11] = Wire.read();
  data->arr8[10] = Wire.read();
  data->arr8[13] = Wire.read();
  data->arr8[12] = Wire.read();
  
  data->arr8[7] = Wire.read();
  data->arr8[6] = Wire.read();
  
  data->arr8[1] = Wire.read();
  data->arr8[0] = Wire.read();
  data->arr8[3] = Wire.read();
  data->arr8[2] = Wire.read();
  data->arr8[5] = Wire.read();
  data->arr8[4] = Wire.read();

  for(u8 i = 0; i < 3; i++){
    data->arr16[i] -= calibrationData.arr16[i];
  }
}

void gyroAverageReads(MPU_Data *reads)
{
  for(u8 read1 = 0; read1 < 32; read1++){
    for(u8 axis = 0; axis < 3; axis++){
  calibrationData.arr16[axis] += reads[read1].arr16[axis]/32;
    }
  }
}

void gyroInit(void)
{
  TWBR = 12; //set I2C communication rate at 400KHz
  Wire.begin();
  gyroWrite8(PWR_MGMT_1, PWR_MGMT_1_DATA);
  gyroWrite8(GYRO_CONFIG, GYRO_CONFIG_DATA);
  gyroWrite8(ACC_CONFIG, ACC_CONFIG_DATA);
  MPU_Data calibrationReads[32] = {0};
  //MPU_Data calibrationAverages = {0};
  for(u8 read1 = 0; read1 < 32; read1++){
    gyroReadXYZ(&calibrationReads[read1]);
  }
  gyroAverageReads(calibrationReads);

}

void gyroPrintData(rotation_axis* data)
{
    const static char *labels[] = {
    "Roll: ",
    "Pitch: ",
  };
  Serial.println("SHITTY DegPS: ");
  for(u8 i = 0; i < 2; i++){
    Serial.print(labels[i]);
    Serial.println(data->arrf[i]);

  }
  Serial.println("");
}

void setup()
{
  Serial.begin(9600);
  MPU_Data angles;
  gyroInit();

  while(1){
    static rotation_axis gyro_values = {0};
    static rotation_axis acc_values = {0};
    static rotation_axis final_values = {0};
    static u32 time1 = 0;
    static u32 time2 = 0;

    
    if(micros() - time1 >= 4000){
      gyroReadXYZ(&angles);
      gyro_values.pitch += angles.gyroX.full*0.0000611f;
      gyro_values.roll += angles.gyroY.full*0.0000611f;
      //gyro_values.roll -= gyro_values.pitch*sin((float)angles.gyroZ.full*0.000001066f);//acounts for yaw change
      //gyro_values.pitch += gyro_values.roll*sin((float)angles.gyroZ.full*0.000001066f);

      //I stole the lines//Accelerometer angle calculations
      acc_total_vector = sqrt(((long)angles.accX.full*(long)angles.accX.full)+((long)angles.accY.full*(long)angles.accY.full)+((long)angles.accZ.full*(long)angles.accZ.full));       //Calculate the total accelerometer vector.
      
      if(abs(angles.accX.full) < acc_total_vector){                                        //Prevent the asin function to produce a NaN
        acc_values.pitch = asin((float)angles.accY.full/acc_total_vector)* 57.296;          //Calculate the pitch angle.
      }
      if(abs(angles.accX.full) < acc_total_vector){                                        //Prevent the asin function to produce a NaN
        acc_values.roll = asin((float)angles.accX.full/acc_total_vector)* -57.296;          //Calculate the roll angle.
      }

      //Place the MPU-6050 spirit level and note the values in the following two lines for calibration.
      acc_values.pitch -= -3.30;                                                   //Accelerometer calibration value for pitch.
      acc_values.roll -= 6.30;                                                    //Accelerometer calibration value for roll.
      
      final_values.pitch = gyro_values.pitch * 0.9996 + acc_values.pitch * 0.0004;            //Correct the drift of the gyro pitch angle with the accelerometer pitch angle.
      final_values.roll = gyro_values.roll * 0.9996 + acc_values.roll * 0.0004;               //Correct the drift of the gyro roll angle with the accelerometer roll angle.
    
      time1 = micros();
    }
    if(micros() -  time2 >= 1000000){
//      Serial.println(angles.accX.full);
//      Serial.println(angles.accY.full);
//      Serial.println(angles.accZ.full);
//      Serial.println(acc_total_vector);
//      Serial.println("");
      gyroPrintData(&acc_values);
      time2 = micros();
    }
  }
}

void loop()
{

}
