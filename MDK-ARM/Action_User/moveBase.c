/**
  ******************************************************************************
  * @file	  moveBase.c
  * @author	  Action
  * @version   V1.0.0
  * @date	  2018/08/09
  * @brief	 2018省赛底盘运动控制部分
  ******************************************************************************
  * @attention
  *			None
  ******************************************************************************
  */
/* Includes -------------------------------------------------------------------------------------------*/

#include "moveBase.h"
#include <app_cfg.h>
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "elmo.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "math.h"
#include "pps.h"
/* Private typedef ------------------------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------------------------*/
/**
  * @brief  
  * @note
  * @param  
  * @retval None
  */

float Kp=38,Ki=0,Kd=0,err=0,lastErr=0,Sumi=0,Output=0,Vk=0,errl,lastErr1;
extern int R;
void Straight(float v)
{
	VelCrl(CAN1,1,-v*CAR_WHEEL_COUNTS_PER_ROUND/(pi*WHEEL_DIAMETER)*REDUCTION_RATIO*WHEEL_REDUCTION_RATIO);
	VelCrl(CAN1,2,-Vk*CAR_WHEEL_COUNTS_PER_ROUND*REDUCTION_RATIO*WHEEL_REDUCTION_RATIO/(pi*TURN_AROUND_WHEEL_DIAMETER));
}	
void Spin(float R,float v)
{
	VelCrl(CAN2,1,v*4096/(pi*WHEEL_DIAMETER)+Vk);
	VelCrl(CAN2,2,-v*4096*(R+(WHEEL_TREAD-WHEEL_WIDTH)/2)/((R-(WHEEL_TREAD-WHEEL_WIDTH)/2)*pi*WHEEL_DIAMETER)+Vk);
}
void TurnRight(float angle,float v)
{
	VelCrl(CAN2,1,0);
	VelCrl(CAN2,2,-v*4096*WHEEL_TREAD*angle*2/((WHEEL_DIAMETER)*360));
}	
void AnglePID(float setAngle,float feedbackAngle)
{
	err=setAngle-feedbackAngle;
	if(err>180)
		err=-(360-err);
	if(err<-180)
		err=360+err;
	Sumi+=Ki*err;
	Vk=Kp*err+Sumi+Kd*(err-lastErr);
	lastErr=err;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									
}	
float k,b,lAngle,setAngle,x,y,d;
int flag;
extern float yawAngle;
extern int status;
void GetFunction(float x1,float y1,float x2,float y2)
{
		if(x1-0.1<x2&&x2<x1+0.1)
		{
			if(y2-y1>0)
				lAngle=0;
			else
				lAngle=180;
			flag=0;
		}
	  	else
		{
			k=(y2-y1)/(x2-x1);
			b=y1-k*x1;
			if(k>0)
			{	
				if(y2-y1>=0)
					lAngle=(atan(k)*180/pi)-90;
				else 
					lAngle=(atan(k)*180/pi)+90;
			}	
			else 
			{
				if(y2-y1>=0)
					lAngle=(atan(k)*180/pi)+90;
				else 
					lAngle=(atan(k)*180/pi)-90;
			}	
			if(y1-0.5<y2&&y2<y1+0.5)
			{	
				if(x2-x1>0)
					lAngle=-90;
				if(x2<x1)
					lAngle=90;
			}	
			flag=1;
		}
}	
void linePID(float x1,float y1,float x2,float y2,float v)
{
		
		GetFunction(x1,y1,x2,y2);
		if(flag)
		{
			if(k>0)
				if((y-k*x-b)*(y2-y1)>0)
					setAngle=lAngle-90*(1-1000/(y-k*x-b+1000));
				else
					setAngle=lAngle+90*(1-1000/(-y+k*x+b+1000));
			else
				if((y-k*x-b)*(y2-y1)>0)
					setAngle=lAngle+90*(1-1000/(y-k*x-b+1000));
				else
					setAngle=lAngle-90*(1-1000/(-y+k*x+b+1000));
			//斜率为0	
			if(y1-0.5<y2&&y2<y1+0.5)
			{	
				if(x2>x1)
					if(y>y1)
						setAngle=lAngle-90*(1-1000/(y-b+1000));
					else
						setAngle=lAngle+90*(1-1000/(-y+b+1000));
				else
					if(y>y1)
						setAngle=lAngle+90*(1-1000/(y-b+1000));
					else
						setAngle=lAngle-90*(1-1000/(-y+b+1000));	
			}	
		}	
		//斜率不存在
	 	else
		{	
			if((y2-y1)*(x-x1)<0)
					setAngle=lAngle-90*(1-1000/(fabs(x-x1)+1000));
			else
					setAngle=lAngle+90*(1-1000/(fabs(x-x1)+1000));	
		}
		AnglePID(setAngle,GetAngle());
		Straight(v);
		
}	
void CirclePID(float x0,float y0,float R,float v,int status)
{
	x=GetX();
	y=GetY();
	GetFunction(x,y,x0,y0);
	d=(x-x0)*(x-x0)+(y-y0)*(y-y0);
	//逆时针
	if(status==0)
	{	
		if(sqrtf(d)>R)
			setAngle=lAngle-90*(500/(fabs(sqrtf(d)-R)+500));	
		else
			setAngle=lAngle-180+90*(500/(fabs(sqrtf(d)-R)+500));		
	}	 
	//顺时针
	if(status==1)
	{	
		if(sqrtf(d)>R)
			setAngle=lAngle+90*(500/(fabs(sqrtf(d)-R)+500));
		else
			setAngle=lAngle+180-90*(500/(fabs(sqrtf(d)-R)+500));
	}	
	AnglePID(setAngle,GetAngle());
	Straight(v);
}	
/********************* (C) COPYRIGHT NEU_ACTION_2018 ****************END OF FILE************************/
extern float angle,T,v,Distance;
void GetYawangle(float x1,float y1)
{
	static float R;
	static float x,y,antiRad;
	//含有误差的实时半径
	antiRad=T*v/R;
	R=sqrtf(GetX()*GetX()+(GetY()-2400)*(GetY()-2400));
	if(status==0)
	{	
		//预判Ts后的坐标（适用于走圆形）
		x=R*cos((angle-90)*pi/180+antiRad);
		y=R*sin((angle-90)*pi/180+antiRad)+2400;
		//预判Ts后炮台坐标
		x=x-68*sin(GetAngle()*pi/180);
		y=y+68*cos(GetAngle()*pi/180);
		GetFunction(x,y,x1,y1);
		if(GetAngle()<-90&&lAngle>90)
			yawAngle=360+GetAngle()-lAngle;
		else
			yawAngle=GetAngle()-lAngle;		
	}
	else
	{
		x=R*cos((angle+90)*pi/180-antiRad);
		y=R*sin((angle+90)*pi/180-antiRad)+2400;
		//预判Ts后炮台坐标
		x=x-68*sin(GetAngle()*pi/180);
		y=y+68*cos(GetAngle()*pi/180);
		GetFunction(x,y,x1,y1);
		if(lAngle<-90&&GetAngle()>90)
			yawAngle=-360+GetAngle()-lAngle;
		else
			yawAngle=GetAngle()-lAngle;	
	}	
}
void GetDistance(float x1,float y1)
{
	static float x,y,antiRad;
	static float R;
	antiRad=T*v/R;
	//含有误差的实时半径
	R=sqrtf(GetX()*GetX()+(GetY()-2400)*(GetY()-2400));
	if(status==0)
	{	
		x=R*cos((angle-90)*pi/180+antiRad);
		y=R*sin((angle-90)*pi/180+antiRad)+2400;
		x=x-68*sin(GetAngle()*pi/180+antiRad);
		y=y+68*cos(GetAngle()*pi/180+antiRad);
	}	
	else
	{
		x=R*cos((angle+90)*pi/180-antiRad);
		y=R*sin((angle+90)*pi/180-antiRad)+2400;
		x=x-68*sin(GetAngle()*pi/180-antiRad);
		y=y+68*cos(GetAngle()*pi/180-antiRad);
	}
	Distance=sqrtf((x-x1)*(x-x1)+(y-y1)*(y-y1));
}	