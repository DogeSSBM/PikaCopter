#include "Includes.h"
extern "C"{
#define GYRO_ADDR		0xD3
// #define ACCL_ADDR

#define GYRO_CTRL1_ADDR	0x20
#define GYRO_CTRL1_DATA	0b0100111
// Bits from left to right:
// Output Data Rate bit 1, Output Data Rate bit 0,
// Bandwidth selection bit 1,Bandwidth selection bit 0,
// Power enable, Z axis enable, Y axis enable, X axis enable

#define GYRO_CTRL2_ADDR
#define GYRO_CTRL2_DATA
// Unused

#define GYRO_CTRL3_ADDR
#define GYRO_CTRL3_DATA
// Unused

#define GYRO_CTRL4_ADDR	0x23
#define GYRO_CTRL4_DATA	0b10000001
// Bits from left to right:
// Block Data Update, 0=LSB 1=MSB,
// Full Scale selection bit 1, Full Scale selection bit 0
// Unused,
// Self Test Enable bit 1, Self Test Enable bit 0,
// Unused (0=4 1=3 Wire SPI)

#define OUT_X_L 		0x28
#define OUT_X_H 		0x29
#define OUT_Y_L 		0x2A
#define OUT_Y_H 		0x2B
#define OUT_Z_L 		0x2C
#define OUT_Z_H 		0x2D
}
// typedef union{
// 	u8 arr[6];
// 	struct{
// 		union{
// 			u16 full;
// 			struct{
// 				u8 low;
// 				u8 high;
// 			};
// 		}x;
//
// 		union{
// 			u16 full;
// 			struct{
// 				u8 low;
// 				u8 high;
// 			};
// 		}y;
//
// 		union{
// 			u16 full;
// 			struct{
// 				u8 low;
// 				u8 high;
// 			};
// 		}z;
//
// 	};
// }GyroData;

static inline
void gyroWrite8(const u8 reg, const u8 data)
{
	Wire.beginTransmission(GYRO_ADDR);
	Wire.write(reg);
	Wire.write(data);
	Wire.endTransmission();
}

void gyroInit(void)
{
	Wire.begin();
	gyroWrite8(GYRO_CTRL_1, GYRO_CTRL_1_DAT);
	gyroWrite8(GYRO_CTRL_4, GYRO_CTRL_4_DAT);
}

// void gyroReadXYZ(GyroData *data)
// {
//
// }

void setup()
{

}

void loop()
{

}
