#include "Includes.h"
#define GYRO_ADDR		0xD3
#define ACCL_ADDR

#define GYRO_CTRL_1	0x20
#define GYRO_CTRL_2	0x21

#define OUT_X_L 		0x28
#define OUT_X_H 		0x29
#define OUT_Y_L 		0x2A
#define OUT_Y_H 		0x2B
#define OUT_Z_L 		0x2C
#define OUT_Z_H 		0x2D


typedef union{
	u8 arr[6];
	struct{
		union{
			u16 full;
			struct{
				u8 low;
				u8 high;
			};
		}x;

		union{
			u16 full;
			struct{
				u8 low;
				u8 high;
			};
		}y;

		union{
			u16 full;
			struct{
				u8 low;
				u8 high;
			};
		}z;

	};
}GyroData;

void gyroInit(void)
{

}

void gyroRead(GyroData *data)
{

}

void setup()
{
	GyroData data = {0};
	gyroRead(&data);

}

void loop()
{

}
