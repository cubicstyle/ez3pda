
#ifndef GLOBAL_H
#define GLOBAL_H

//�����Ez3��һЩ���á�
#define	_Ez3PsRAM 			0x08400000
#define	_Ez3NorList			0x083c0000
#define _Ez3NorRom			0x09400000
#define _Ez3NorRomEnd		0x09C00000
#define	_UnusedVram 		0x06012c00
#define	_UnusedVramSize 	0x00005400
#define _UnusedEram			0x02013c00
#define _ramFatCache		0x02000000
#define _ramFatBlockCache	0x02020000
#define _psFatCache			(_Ez3PsRAM+0xC00000)
#define _psFatBlockCache	(_Ez3PsRAM+0xC20000)
#define _Ez3NorCNName       0x08028000			//�����б�����
#define _Ez4NorCNName		0x083E0000
#define MAXDISKNAME			32
#define _GoldenEnable		0x0E008400
#define _GoldenSaver		0x0E008200
#define _FileInfoSaver		0x0E008000
#define _SRAMSaver		0x0E000000

#define _txtBaseSaver		0x0E008404
#define _txtSaver1			0x0E008404
#define _txtSaver2			0x0E008504
#define _txtSaver3			0x0E008604
#define _txtSaver4			0x0E008704


//����Nor�е��ļ��б�
typedef struct FM_NOR_FILE_SECT{	////�ļ����������ļ���ʾ�б�Ԫ���ݽṹ
	char filename[256];
	u16 rompage ;
	u16 saversize ;
	u16	savertype;
	u16 reserved ;
	u32 filesize;
	u32 reserved2 ;
} FM_NOR_FS;

//
#define GAMEPAKMem	   (u8*) 0x8000000
#define GAMEPAKRES	GAMEPAKMem  //ͼ����Դλ��
#define GBAPAKLINE	(GAMEPAKMem + 256*1024)  //�ֿ���ļ���Դλ��

#define SAVEPOINT	0xE000000				//���̵�
#define SAVEHEADVAL	0x1616
#define SAVEHEAD	SAVEPOINT
#define SAVEISSET	(SAVEPOINT+4)
#define SAVETXTPOS	(SAVEPOINT+8)

#define WORDSAVEPOINT	0x800				//����ѧϰ���̵�


#define MAX_PATH	260
#define HEADSIZE	MAX_PATH+12

//���崰����ɫ����

#define FM_CL_BG			RGB(31,31,31)		//һ�㱳��ɫ,��
#define FM_CL_TITLE			RGB(8,16,25)			//���ڱ�����,��ɫ


#define FM_CL_WHITE			RGB(31,31,31)		//һ�㱳��ɫ,��
#define FM_CL_BLACK			RGB(3,0,0)			//��ɫ
#define FM_CL_HIGHBLUE		RGB(19,23,27)			//����
#define FM_CL_BLUE			RGB(4,8,16)			//����
#define FM_CL_HIGHGRAY		RGB(28,28,28)			//����
#define FM_CL_BLUEGRAY		RGB(12,20,22)			//����
#define FM_CL_GRAY			RGB(23,23,23)			//��ɫ
#define FM_CL_DUKGRAY		RGB(17,17,17)			//���
#define FM_CL_YELLOW		RGB(31,31,8)			//��ɫ
#define FM_CL_RED			RGB(31,1,1)			//��ɫ

//ʱ���趨�ı���
#define TM_CL_BG            0x3F9F

//���浱ǰ��������
#define DESKTOP_FLAG_NULL		0
#define DESKTOP_FLAG_CREAT		1
#define DESKTOP_FLAG_UPD		2
#define DESKTOP_FLAG_OVER		9



//�ļ��������ǰ��������
#define SHELL_FLAG_NULL			0			//������
#define SHELL_FLAG_CREATE		1			//�´���
#define SHELL_FLAG_RESHOW		2			//�������򷵻ص������
#define SHELL_FLAG_SCR			3			//���������
#define SHELL_FLAG_CHG			4			//����������ļ�����

//TXT�Ķ�����ǰ��������
#define TXT_FLAG_NULL			0			//������
#define TXT_FLAG_CREATE			1			//�´���
#define TXT_FLAG_RESHOW			2			//�������򷵻ص�txt
#define TXT_FLAG_SCR			3			//TXT����

#define TXT_ROW_H			13			//TXT��������±��и�
#define TXT_MSN				12			//TXT�������ÿҳ��ʾ����
#define TXT_L				1			//TXT�����������������߽�
#define TXT_W				230			//TXT�����������������
#define TXT_T				2			//TXT����������������ϱ߽�
#define TXT_H				156			//TXT���������������߶�

//JPG�Ķ�����ǰ��������
#define JPG_FLAG_NULL			0			//������
#define JPG_FLAG_CREATE			1			//�´���


#define FM_TITLE_H			23			//���ڱ�������
#define FM_FOOT_H			13			//����״̬����
#define FM_SCR_W			6			//�����϶�����

#define FM_ROW_H			17			//������ļ��и�
#define FM_F_NAME_X			20			//������ļ�����ʾX
#define FM_F_NAME_W			138			//������ļ�����ʾ���
#define FM_F_SIZE_X2		216			//������ļ���С��ʾλ��
#define FM_F_SIZE_W			80			//������ļ���С��ʾ���
#define FM_FILE_MAX_SHOW_NUM	8
#define FM_MSN	FM_FILE_MAX_SHOW_NUM		//��������ͬʱ��ʾ����


//��ʾ�ļ������ʱ��ѡ��ˢ����һ����
#define FM_MD_NULL			0x0			//��
#define FM_MD_HEAD			0x1			//ˢ�������ͷ
#define FM_MD_FOOT			0x2			//ˢ�������β
#define FM_MD_FILES			0x4			//ˢ���ļ���
#define FM_MD_SCR			0x8			//ˢ���϶���
#define FM_MD_FOCUS			0x10		//ˢ��ѡ���ļ�
#define FM_MD_ALL			FM_MD_HEAD|FM_MD_FOOT|FM_MD_FILES|FM_MD_SCR|FM_MD_FOCUS		//ˢ��ȫ��
//��ʾTXT�ļ�ʱ��ѡ��ˢ����һ����
#define FM_TXT_NULL			0x0			//��
#define FM_TXT_LINES			0x1			//ˢ��ȫ���ı���
#define FM_TXT_SPCLINE			0x2			//ˢ�������У�ˢ�µ��У�




typedef struct FM_MD_FILE_SECT{	////�ļ����������ļ���ʾ�б�Ԫ���ݽṹ
	char filename[64];
	u8 isDir;
	u32 filesize;
} FM_MD_FS;
typedef struct FM_MD_FILE{		//�ļ����������ļ���ʾ�б����ݽṹ
	u16 sectnum;
	FM_MD_FILE_SECT sect[FM_MSN];
}FM_MD_F;

typedef struct FILE_SAVE_INFO{	//�ýṹ�嶨�屣���û��Ķ����̵����Ϣ
	u32 flg ;
	u16 index;
	char path[128];
	char fname[64];
	u16 fi;
	u16 viewfi;
	u16 pos;
	u32 pos2;
}FILE_SI;


typedef struct res_Struct
{
	u8 *res_ZKDATA;		//���ֿ�
	u8 *res_ASCDATA;	//ASC�ֿ�
	u16 *res_DESKBG;	//���汳��
	u16 *res_DESKICON;	//����ͼ��
	u16 *res_FILEICON;	//�ļ�ͼ��
	u16 *res_FILEBG;	//�ļ�����������
	u16 *res_FILEBGHEAD;//�ļ�����������ͷ
	u16 *res_TXTBG;		//���±�����
	u16 *res_TXTSCUP;	//���±�������
	u16 *res_TXTSCDOWN;	//���±�������
	u16 *res_TXTSCSIGN;	//���±�����
	u16 *res_TXTSCBACK;	//���±���������
}res_struct;



#endif



