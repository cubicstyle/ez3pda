#include "agbTypes.h"

#define CLOCK_DEVICE	0x080000C8
#define CLOCK_CONTROL	0x080000C6
#define CLOCK_DATA		0x080000C4
#define	CLOCK_CSOUT		0x4
#define	CLOCK_SIOOUT	0x2
#define	CLOCK_SCKOUT	0x1
#define CLOCK_SCK		0x1
#define CLOCK_SIO		0x2
#define CLOCK_CS 		0x4		

#define FlashBase		0x09400000
#define FlashWindow		0x00800000		

#define		SETW(adr, val) (*((volatile u16*)adr) = val)

typedef struct _Time
{
	u8 year;
	u8 month;
	u8 data;
	u8 week;
	u8 hour;
	u8 minute;
	u8 second;
	u8 res;
}CLOCK_TIME;

/*
�й�EZ4 ���ķ���
1��128PSRAM��256MNOR
	1��2ƬVZ064�� 128MPSRAM�� 4X64MNOR
	2��1ƬVZ128�� 128MPSRAM�� 2X128MNOR
	3��1ƬH6H6H6��128MPSRAM�� 2X128MNOR 
	
2����3��NOR�ṹ��ͬ��NOR��ID ��ͬ
2��256MPSRAM�� 512M NOR
	1��2ƬVZ128���������ͣУӣң��ͣ������أ������͡��Σϣ�
	����2Ƭ�ȣ��ȣ��ȣ����������ͣУӣң��ͣ������أ������͡��Σϣ�
	��	�ṹ��ͬ�����ɣĲ�ͬ
*/
typedef enum {
	EZ4_NoCard=-1,
	EZ4_4Chip,
	EZ4_2Chip,
	EZ4_4ChipPsram,	//256M PSRAM
	EZ4_1Chip
}EZ4_CARTTYPE;

void		_SetEZ2Control(u32* FuncAddr ,u16 control);
void		SetEZ2Control(u16 control);
void		GetTime(CLOCK_TIME *cs) ;
void		SetTime(CLOCK_TIME *cs) ;
void 		GetTime_Orignal(CLOCK_TIME *cs);
void		Set24Hour();
void 		Clock_Enable();
void 		Clock_Disable();
void 		Clock_DisableReadWrite();
void  		OpenRamWrite();
void  		CloseRamWrite();

u32 chip_id();
void chip_reset();
void chip_erase(u32 id);
void WriteFlash(u32 id ,u32 address, u8* buffer,u32 size);

//enable write
void		OpenWrite();
void		CloseWrite();

EZ4_CARTTYPE	CheckEz4Card();
void Block_Erase(u32 blockAdd);
void WriteEZ4Flash(u32 address,u8 *buffer,u32 size);
void SetSerialMode();