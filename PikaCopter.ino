#include "Includes.h"
#define GYRO_ADDR		0x69
// #define ACCL_ADDR

#define GYRO_CTRL1_ADDR	0x20
#define GYRO_CTRL1_DATA	0x0F
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

#define GYRO_CTRL4_ADDR	0x23
#define GYRO_CTRL4_DATA	0b10000000
// Bits from left to right:
// Block Data Update, 0=LSB 1=MSB,
// Full Scale selection bit 1, Full Scale selection bit 0
// Unused,
// Self Test Enable bit 1, Self Test Enable bit 0,
// Unused (0=4 1=3 Wire SPI)

#define OUT_ALL 		(0x28|0x80)
#define OUT_X_L 		0x28
#define OUT_X_H 		0x29
#define OUT_Y_L 		0x2A
#define OUT_Y_H 		0x2B
#define OUT_Z_L 		0x2C
#define OUT_Z_H 		0x2D

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
	for(u8 i = 0; i < 6; i++){
		data->arr8[i] = Wire.read();
	}
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
	Wire.begin();
	gyroWrite8(GYRO_CTRL1_ADDR, GYRO_CTRL1_DATA);
	gyroWrite8(GYRO_CTRL4_ADDR, GYRO_CTRL4_DATA);
	GyroData calibrationReads[32] = {0};
	//GyroData calibrationAverages = {0};
	for(u8 read1 = 0; read1 < 32; read1++){
		gyroReadXYZ(&calibrationReads[read1]);
	}
	gyroAverageReads(calibrationReads);

}

void gyroPrintData(GyroData *data)
{
	const static char *labels[] = {
		"X: ",
		"Y: ",
		"Z: "
	};
	Serial.println("RAW\t\t\t SHITTY DegPS: ");
	for(u8 i = 0; i < 3; i++){
		Serial.print(labels[i]);
		Serial.print(data->arr16[i]);
		Serial.print("\t\t\t");
		Serial.println(data->arr16[i]/57);
	}
}

void setup()
{
	Serial.begin(9600);
	GyroData info;
	gyroInit();

	while(1){
		gyroReadXYZ(&info);
		gyroPrintData(&info);
		delay(500);
	}

}

void loop()
{

}
