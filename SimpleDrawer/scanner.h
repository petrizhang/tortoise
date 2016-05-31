#pragma once

#define True 1
#define False 0
#define BOOL unsigned 
#define MAX_TOKEN_SIZE 100//token�ַ�������󳤶�Ϊ100
#define ID_TAB_TOTALS 18//id���Ԫ�ظ���
#define TOKEN_TYPE_TOTALS 23//����23�ּǺ�����

extern unsigned int lineNo;//�к�

// ��������
enum ScannerErrCode
{
	SCANNER_OK,
	OPEN_FILE_FAILED,
	TOKEN_OVERFLOW,
	INVALID_TOKEN
};

//�Ǻ�����
enum TokenType
{
	ORIGIN, SCALE, ROT, IS,	  // �����֣�һ��һ�룩
	TO, STEP, DRAW,FOR, FROM, // ������
	T,				  // ����
	SEMICO, L_BRACKET, R_BRACKET, COMMA,// �ָ���
	PLUS, MINUS, MUL, DIV, POWER,		// �����
	FUNC,				  // ���������ã�
	CONST_ID,			  // ����
	NONTOKEN,			  // �ռǺţ�Դ���������
	ERRTOKEN			  // ����Ǻţ��Ƿ����룩
};

// token�ṹ��Ķ���
typedef struct {
	enum TokenType token_type;//�Ǻ�����
	char* token_string;//�Ǻ��ַ���
	double value;//�Ǻ�ֵ
	double (*FUNC_PTR)(double);//����ָ�룬ֻ֧�ֵ�double������double����ֵ�ĺ���
} Token;

//�Ϸ�id��
static Token id_tab[ID_TAB_TOTALS] =
{
	{ CONST_ID,	"PI",		3.1415926,	NULL },
	{ CONST_ID,	"E",		2.71828,	NULL },
	{ T,		"T",		0.0,		NULL },
	{ FUNC,		"SIN",		0.0,		sin  },
	{ FUNC,		"COS",		0.0,		cos  },
	{ FUNC,		"TAN",		0.0,		tan  },
	{ FUNC,		"LN",		0.0,		log  },
	{ FUNC,		"EXP",		0.0,		exp  },
	{ FUNC,		"SQRT",		0.0,		sqrt },
	{ ORIGIN,	"ORIGIN",	0.0,		NULL },
	{ SCALE,	"SCALE",	0.0,		NULL },
	{ ROT,		"ROT",		0.0,		NULL },
	{ IS,		"IS",		0.0,		NULL },
	{ FOR,		"FOR",		0.0,		NULL },
	{ FROM,		"FROM",		0.0,		NULL },
	{ TO,		"TO",		0.0,		NULL },
	{ STEP,		"STEP",		0.0,		NULL },
	{ DRAW,		"DRAW",		0.0,		NULL }
};

//Ϊ�����ӡ��ÿ�����Ͷ�Ӧ���ַ���
static char* token_repr[TOKEN_TYPE_TOTALS] =
{
	"ORIGIN", "SCALE", "ROT", "IS",	  // �����֣�һ��һ�룩
	"TO", "STEP", "DRAW","FOR", "FROM", // ������
	"T",				  // ����
	"SEMICO", "L_BRACKET", "R_BRACKET", "COMMA",// �ָ���
	"PLUS", "MINUS", "MUL", "DIV", "POWER",		// �����
	"FUNC",				  // ���������ã�
	"CONST_ID",			  // ����
	"NONTOKEN",			  // �ռǺţ�Դ���������
	"ERRTOKEN"			  // ����Ǻţ��Ƿ����룩
};


//-----------------------��ʽ����ӡtoken
void printToken(Token token);

//-----------------------��ʼ���ʷ�������
ScannerErrCode initScanner(const char* fileName);

//-----------------------�رմʷ�������
void closeScanner();

//-----------------------��ȡһ���﷨�Ǻ�
Token getToken();

//-----------------------scanner�Ĳ��Ժ���
void scanner(const char* fileName);
