/****************************************Copyright (c)****************************************************
**
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               app_cfg.h
** Descriptions:            ucosii configuration
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__
#include <os_cpu.h>
#include <stdint.h>
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "adc.h"
#include "elmo.h"
#include "moveBase.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "moveBase.h"  

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

//用于串口发数
typedef struct {
	float distance;
	float turnAngleValue;//
	uint8_t flagValue;
	float shootangle;
	float shootSp;
	uint8_t shootFlg;
	uint8_t flgOne;
	uint8_t errflg;
	float judgeSp;
	uint8_t ready[4];
	uint8_t ball;
	
}usartValue;


extern  void  App_Task(void);

static  void  App_TaskStart(void);
static 	void  ConfigTask(void);
static 	void  WalkTask(void);
static	void  Init(void);
static	void  PosConfig(void);

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO						10u
#define  Config_TASK_START_PRIO						11u
#define  Walk_TASK_PRIO								12u




/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_TASK_START_STK_SIZE					256u
#define  Config_TASK_START_STK_SIZE					1024u
#define  Walk_TASK_STK_SIZE							2048u

/*
*********************************************************************************************************
*                                            TASK STACK
*
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/



#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

