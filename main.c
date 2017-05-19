#include <STC12C5A.H>
#include <stdio.h>
#include <STC12C5A.H>
#include "Aircharger_V1_Interface.h"

//全局变量
unsigned char task_number = 0;		//任务号
unsigned char is_timer_init = 1;	//计时器第一次启动标志
sbit Aplus = P2^7;
void main()
{	
	
	System_Init();//系统初始化
	delayms(2000);//延时，等待液晶屏启动
	First_Launch();//执行初次开机的一些参数
	Read_Settings_From_Eeprom();//从EEPROM中读取配置
	Reset_Pressing_Motor();//重置压紧电机
	Tasks_Init();//任务初始化
	Watchdog_Init();
	EA = 1;
	ET1 = 1;
	TMOD |= 1;
	TH1 = 0xCF;
	TL1 = 0x0F;
	IPH = PT1H;	  //提升任务调度计时器优先级
	PT1 = 1;
	TR1 = 1; 
//多任务系统设置完毕
	while(1);
}
void timer1() interrupt 3{
	//static char timer_delay = 0;
	
	//if (timer_delay >= 2){
	//	timer_delay = 0;
	
	switch(task_number){
		case 0:	   //任务0
			#pragma asm
				using 0
				MOV		R2,		#0		  //任务0下调基地址0字节
			#pragma endasm
		break;
		case 1:	   //任务1
			#pragma asm
				using 0
				MOV		R2,		#1		  //任务1下调基地址1×15字节
			#pragma endasm
		break;
		case 2:	   
			#pragma asm
				using 0
				MOV		R2,		#2		  //任务2下调基地址2×15字节
			#pragma endasm
		break;
		case 3:	  
			#pragma asm
				using 0
				MOV		R2,		#3		  //任务3下调基地址3×15字节
			#pragma endasm
		break;
		case 4:	   
			#pragma asm
				using 0
				MOV		R2,		#4		  //任务4下调基地址4×15字节
				//MOV		030H,	#0x11
			#pragma endasm
		break;
		
		case 5:	   
			#pragma asm
				using 0
				MOV		R2,		#5		  //任务5下调基地址5×15字节
			#pragma endasm
		break;
	}
		if(is_timer_init){
			#pragma asm						//如果是第一次进入计时器，则不保存task0现场（因为task0未被执行过）
				using 0
				MOV		DPH,	#0x03		//设定数据指针高8位为0x03
				MOV		B,		#15			//保存区块单位大小为15B
				MOV		A,		R2
				MUL		AB
				MOV		R2,		A
				MOV		A,		#0xF0
				SUBB	A,		R2
				MOV		DPL,	A
				MOV 	R0,	  	#0
				LJMP    NEXT_TASK_LOAD
			#pragma endasm
			}
		else{
			#pragma asm					//从堆栈保存前一任务的数据，从固定地址提取下一任务数据，装载到堆栈
				using 0
				MOV		DPH,	#0x03		//设定数据指针高8位为0x3
				MOV		B,		#15		//保存区块单位大小为15B
				MOV		A,		R2
				MUL		AB
				MOV		R3,		A
				MOV		A,		#0xF1
				SUBB	A,		R3
				MOV		DPL,	A
				MOV 	R0,	  	#0
			#pragma endasm
			}
			#pragma asm
			PREVIOUS_TASK_SAVE:			  //保存先前任务数据操作
				MOV		R1,	  	SP
				MOV		A,		@R1
			    MOVX	@DPTR,	A
				DEC 	SP
				INC		DPTR
				INC		R0
				CJNE	R0,	  	#14,		PREVIOUS_TASK_SAVE

				CJNE	R2,		#5,			NEXT_TASK		  //如果task_number不是5，则装载下一任务

				MOV		DPH,	#0x03
				MOV		DPL,	#0xFF
				MOV		R0,		#0
			TASK5_TO_TASK0:						//如果前任务为最后任务，则装载任务0到堆栈，构成任务轮转
				MOVX 	A,		@DPTR
				MOV		R1,		SP
				MOV		@R1,	A
				INC		SP
				MOV		A,		#-1
				ADD		A,		DPL
				MOV		DPL,	A
				INC		R0
				CJNE	R0,		#14,		TASK5_TO_TASK0
				SJMP	OVER

			NEXT_TASK:						  //装载下一任务数据到堆栈操作
				MOV		A,		#-15
				ADD		A,		DPL
				MOV		DPL,	A
				MOV		R0,		#0
			NEXT_TASK_LOAD:
				MOVX 	A,		@DPTR
				MOV		R1,		SP
				MOV		@R1,	A
				INC		SP
				MOV		A,		#-1
				ADD		A,		DPL
				MOV		DPL,	A
				INC		R0
				CJNE	R0,		#14,		NEXT_TASK_LOAD
			OVER:
			#pragma endasm
			is_timer_init = 0;			  //更新计时器初始化旗标
			task_number++;				  //更新任务号
		if(task_number > 5){			  
			task_number = 0;
		}
	//}
	//else{
	//	timer_delay++;}
	Aplus ^= 1;
	TL1 = 0x00;
	TH1 = 0x09;
	//TR1 = 1;
}	