#include "Includes.h"
#define GYRO_ADDR    0x68
// 0x68
// #define ACCL_ADDR

#define PWR_MGMT_1        0x6B
#define PWR_MGMT_1_DATA   0x00
// Bits from left to right:
// Output Data Rate bit 1, Output Data Rate bit 0,
// Bandwidth selection bit 1, Bandwidth selection bit 0,
// Power enable, Z axis enable, Y axis enable, X axis enable

#define GYRO_CTRL2_ADDR
#define GYRO_CTRL2_DATA
// Unused

#define GYRO_CTRL3_ADDR
#define GYRO_CTRL3_DATA
// Unused

#define GYRO_CONFIG       0x1B
#define GYRO_CONFIG_DATA  0x08
// Bits from left to right:
// Block Data Update, 0=LSB 1=MSB,
// Full Scale selection bit 1, Full Scale selection bit 0
// Unused,
// Self Test Enable bit 1, Self Test Enable bit 0,
// Unused (0=4 1=3 Wire SPI)

#define OUT_ALL     0x43
#define OUT_X_H     0x43
#define OUT_X_L     0x44
#define OUT_Y_H     0x45
#define OUT_Y_L     0x46
#define OUT_Z_H     0x47
#define OUT_Z_L     0x48

typedef union{
	i8 arr8[6];
	i16 arr16[3];
	struct{
		union{
			i16 full;
			struct{
				i8 high;
				i8 low;
			};
		}x;

		union{
			i16 full;
			struct{
				i8 high;
				i8 low;
			};
		}y;

		union{
			i16 full;
			struct{
				i8 high;
				i8 low;
			};
		}z;

  };
}GyroData;

  typedef union{
	float arrf[2];
	struct{
	float pitch;
	float roll;
	};
  }rotation_axis;

GyroData calibrationData;

static inline
void gyroWrite8(const u8 reg, const u8 data)
{
  Wire.beginTransmission(GYRO_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void gyroReadXYZ(GyroData *data)
{
  Wire.beginTransmission(GYRO_ADDR);
  Wire.write(OUT_ALL);
  Wire.endTransmission();
  Wire.requestFrom(GYRO_ADDR, 6);
  while(Wire.available()<6);

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

void gyroAverageReads(GyroData *reads)
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
  GyroData calibrationReads[32] = {0};
  //GyroData calibrationAverages = {0};
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
	GyroData angles;
	gyroInit();

	while(1){
		static rotation_axis rotation_values = {0};
		u32 time1 = 0;
		u32 time2 = micros();
		if(time2 - time1 >= 4000){
			gyroReadXYZ(&angles);
			rotation_values.pitch += (float)angles.x.full*0.0000611f;
			rotation_values.roll += (float)angles.y.full*0.0000611f;
			time1 = time2;
			gyroPrintData(&rotation_values);
		}
	}
}

void loop()
{

}
