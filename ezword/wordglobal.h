
#ifndef WORDGLOBAL_H
#define WORDGLOBAL_H

#define RUN_ENV_REAL			//GBA����,�����κ���Ϊģ��������
#define	NO_LOGO_DBG				//���ڵ���״̬������ʾLOGO

#define PARKFLAG				"EZPDA"
#define WORDLEN		32



/////////////////////////////////////////////////////////////////////////////////
//
//	OLD_APP_�ļ��ṹ
//
//  ORG			INT			HZK			INT			headsect1		ciku1		trans1		headsect2		ciku1		trans2	... ...	
//	ԭʼ����	HZKSIZE		���ֿ�����	�ʿ���Ŀ	�ʿ�1ͷ�ṹ		�ʿ�1		�ʿ�1����	�ʿ�1ͷ�ṹ		�ʿ�1		�ʿ�1����
//

//
//	NEW_LIB_�ļ��ṹ
//
//  ORG			INT			headsect1		ciku1		trans1		headsect2		ciku1		trans2	... ...	
//	�ļ�ϵͳ	�ʿ���Ŀ	�ʿ�1ͷ�ṹ		�ʿ�1		�ʿ�1����	�ʿ�1ͷ�ṹ		�ʿ�1		�ʿ�1����
//



typedef struct headsect{
	u16 wordnum;	
	u16 wordnum2;	
	unsigned int translen;
	unsigned int reserved2;
	unsigned int reserved3;
} ;

typedef struct wordsect{
	char word[WORDLEN];
	unsigned int size;
	unsigned int address;
} ;
typedef struct softhead{
	char data1[256];
	char data2[256];
	char data3[256];
	char data4[256];
} ;

#endif

