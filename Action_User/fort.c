/**
  ******************************************************************************
  * @file     
  * @author  Dantemiwa
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *  
  *
  *
  * 
  ******************************************************************************
  */ 
/* Includes -------------------------------------------------------------------*/

#include "fort.h"
#include "stm32f4xx_usart.h"
#include "string.h"
#include "timer.h"
#include "Pos.h"


//对应的收发串口
#define USARTX UART5
#define FRONT_ANGLE_4_LAUNCHER 170
#define K_OF_LASER 2.4973f
#define B_OF_LASERA 40
#define B_OF_LASERB 360


typedef union
{	
	uint8_t data8[4];
	int			data32;
	float   dataFloat;
}usartData_t;

static struct
{
	usartData_t usartTransmitData;
	usartData_t usartReceiveData;
	float yawPosReceive;
	float shooterVelReceive;
	float laserAValueReceive;
	float laserBValueReceive; 
}fort;



int bufferI = 0;
char buffer[20] = {0};

float Car2LauncherCCS(float);

/**
* @brief 炮台航向控制
* @param  ang:转台航向角度，范围为0~360度
* @retval none
* @attention none
*/
void YawPosCtrl(float ang)
{
    ang = Car2LauncherCCS(ang);
    fort.usartTransmitData.dataFloat = ang;
    USART_SendData(USARTX,'Y');
    USART_SendData(USARTX,'A');
    USART_SendData(USARTX,fort.usartTransmitData.data8[0]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[1]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[2]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[3]);
    USART_SendData(USARTX,'\r');
    USART_SendData(USARTX,'\n');
    fort.usartTransmitData.data32 = 0;
		
}

/**
* @brief 发射电机转速控制
* @param  rps:发射电机速度，单位转每秒
* @retval none
* @attention none
*/
void ShooterVelCtrl(float rps)
{
    fort.usartTransmitData.dataFloat = rps;
    USART_SendData(USARTX,'S');
    USART_SendData(USARTX,'H');
    USART_SendData(USARTX,fort.usartTransmitData.data8[0]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[1]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[2]);
    USART_SendData(USARTX,fort.usartTransmitData.data8[3]);
    USART_SendData(USARTX,'\r');
    USART_SendData(USARTX,'\n');
    fort.usartTransmitData.data32 = 0;
}


void bufferInit()
{
	for(int i = 0; i < 20; i++)
	{
		buffer[i] = 0;
	}
	bufferI = 0;
}

/**
* @brief 接收炮台返回的数据
* @param data：串口每次中断接收到的一字节数据
* @retval none
* @attention 该函数请插入到对应的串口中断中
							注意清除标志位
*/
void GetValueFromFort(uint8_t data)
{
	buffer[bufferI] = data;
	bufferI++;
	if(bufferI >= 20)
	{
		bufferInit();
	}
	if(buffer[bufferI - 2] == '\r' && buffer[bufferI - 1] == '\n')
	{ 
		if(bufferI > 2 &&  strncmp(buffer,"PO",2) == 0)//接收航向位置
		{
				for(int i = 0; i < 4; i++)
					fort.usartReceiveData.data8[i] = buffer[i + 2];
				fort.yawPosReceive = fort.usartReceiveData.dataFloat;
		}
		else if(bufferI > 2 &&  strncmp(buffer,"VE",2) == 0)//接收发射电机转速
		{
				for(int i = 0; i < 4; i++)
					fort.usartReceiveData.data8[i] = buffer[i + 2];
				fort.shooterVelReceive = fort.usartReceiveData.dataFloat;
		}
		else if(bufferI > 2 && strncmp(buffer,"LA",2) == 0)//接收A激光的ADC值
		{
			for(int i = 0; i < 4; i++)
					fort.usartReceiveData.data8[i] = buffer[i + 2];
				fort.laserAValueReceive = fort.usartReceiveData.dataFloat;
		}
		else if(bufferI > 2 && strncmp(buffer,"LB",2) == 0)//接收B激光的ADC值
		{
			for(int i = 0; i < 4; i++)
					fort.usartReceiveData.data8[i] = buffer[i + 2];
				fort.laserBValueReceive = fort.usartReceiveData.dataFloat;
		}
		bufferInit();
	}
}

float ReadShooterVel(void)
{
	return fort.shooterVelReceive;
}

float Launcher2CarCCS(float);
float ReadYawPos(void)
{
	return Launcher2CarCCS(fort.yawPosReceive);
}
float ReadLaserAValue(void)
{
	return K_OF_LASER * fort.laserAValueReceive + B_OF_LASERA;
}
float ReadLaserBValue(void)
{
	return K_OF_LASER * fort.laserBValueReceive + B_OF_LASERB;
}

float Launcher2CarCCS(float angle)
{
    angle = FRONT_ANGLE_4_LAUNCHER - angle + GetA();
    if(angle >= 180)
    {
        angle -= 360;
    }
    else if(angle < -180)
    {
        angle += 360;
    }
    return angle;
}

float Car2LauncherCCS(float angle)
{
    angle = FRONT_ANGLE_4_LAUNCHER - angle + GetA();
    if(angle >= 360)
    {
        angle -= 360;
    }
    else if(angle < 0)
    {
        angle += 360;
    }
    return angle;
}
