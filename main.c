#include <STC12C5A.H>
#include <stdio.h>
#include <STC12C5A.H>
#include "Aircharger_V1_Interface.h"

//ȫ�ֱ���
unsigned char task_number = 0;		//�����
unsigned char is_timer_init = 1;	//��ʱ����һ��������־
sbit Aplus = P2^7;
void main()
{	
	
	System_Init();//ϵͳ��ʼ��
	delayms(2000);//��ʱ���ȴ�Һ��������
	First_Launch();//ִ�г��ο�����һЩ����
	Read_Settings_From_Eeprom();//��EEPROM�ж�ȡ����
	Reset_Pressing_Motor();//����ѹ�����
	Tasks_Init();//�����ʼ��
	Watchdog_Init();
	EA = 1;
	ET1 = 1;
	TMOD |= 1;
	TH1 = 0xCF;
	TL1 = 0x0F;
	IPH = PT1H;	  //����������ȼ�ʱ�����ȼ�
	PT1 = 1;
	TR1 = 1; 
//������ϵͳ�������
	while(1);
}
void timer1() interrupt 3{
	//static char timer_delay = 0;
	
	//if (timer_delay >= 2){
	//	timer_delay = 0;
	
	switch(task_number){
		case 0:	   //����0
			#pragma asm
				using 0
				MOV		R2,		#0		  //����0�µ�����ַ0�ֽ�
			#pragma endasm
		break;
		case 1:	   //����1
			#pragma asm
				using 0
				MOV		R2,		#1		  //����1�µ�����ַ1��15�ֽ�
			#pragma endasm
		break;
		case 2:	   
			#pragma asm
				using 0
				MOV		R2,		#2		  //����2�µ�����ַ2��15�ֽ�
			#pragma endasm
		break;
		case 3:	  
			#pragma asm
				using 0
				MOV		R2,		#3		  //����3�µ�����ַ3��15�ֽ�
			#pragma endasm
		break;
		case 4:	   
			#pragma asm
				using 0
				MOV		R2,		#4		  //����4�µ�����ַ4��15�ֽ�
				//MOV		030H,	#0x11
			#pragma endasm
		break;
		
		case 5:	   
			#pragma asm
				using 0
				MOV		R2,		#5		  //����5�µ�����ַ5��15�ֽ�
			#pragma endasm
		break;
	}
		if(is_timer_init){
			#pragma asm						//����ǵ�һ�ν����ʱ�����򲻱���task0�ֳ�����Ϊtask0δ��ִ�й���
				using 0
				MOV		DPH,	#0x03		//�趨����ָ���8λΪ0x03
				MOV		B,		#15			//�������鵥λ��СΪ15B
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
			#pragma asm					//�Ӷ�ջ����ǰһ��������ݣ��ӹ̶���ַ��ȡ��һ�������ݣ�װ�ص���ջ
				using 0
				MOV		DPH,	#0x03		//�趨����ָ���8λΪ0x3
				MOV		B,		#15		//�������鵥λ��СΪ15B
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
			PREVIOUS_TASK_SAVE:			  //������ǰ�������ݲ���
				MOV		R1,	  	SP
				MOV		A,		@R1
			    MOVX	@DPTR,	A
				DEC 	SP
				INC		DPTR
				INC		R0
				CJNE	R0,	  	#14,		PREVIOUS_TASK_SAVE

				CJNE	R2,		#5,			NEXT_TASK		  //���task_number����5����װ����һ����

				MOV		DPH,	#0x03
				MOV		DPL,	#0xFF
				MOV		R0,		#0
			TASK5_TO_TASK0:						//���ǰ����Ϊ���������װ������0����ջ������������ת
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

			NEXT_TASK:						  //װ����һ�������ݵ���ջ����
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
			is_timer_init = 0;			  //���¼�ʱ����ʼ�����
			task_number++;				  //���������
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