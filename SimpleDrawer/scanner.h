#pragma once

#define True 1
#define False 0
#define BOOL unsigned 
#define MAX_TOKEN_SIZE 100//token字符串的最大长度为100
#define ID_TAB_TOTALS 18//id表的元素个数
#define TOKEN_TYPE_TOTALS 23//共有23种记号类型

extern unsigned int lineNo;//行号

// 错误类型
enum ScannerErrCode
{
	SCANNER_OK,
	OPEN_FILE_FAILED,
	TOKEN_OVERFLOW,
	INVALID_TOKEN
};

//记号类型
enum TokenType
{
	ORIGIN, SCALE, ROT, IS,	  // 保留字（一字一码）
	TO, STEP, DRAW,FOR, FROM, // 保留字
	T,				  // 参数
	SEMICO, L_BRACKET, R_BRACKET, COMMA,// 分隔符
	PLUS, MINUS, MUL, DIV, POWER,		// 运算符
	FUNC,				  // 函数（调用）
	CONST_ID,			  // 常数
	NONTOKEN,			  // 空记号（源程序结束）
	ERRTOKEN			  // 出错记号（非法输入）
};

// token结构体的定义
typedef struct {
	enum TokenType token_type;//记号类型
	char* token_string;//记号字符串
	double value;//记号值
	double (*FUNC_PTR)(double);//函数指针，只支持单double参数单double返回值的函数
} Token;

//合法id表
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

//为方便打印，每个类型对应的字符串
static char* token_repr[TOKEN_TYPE_TOTALS] =
{
	"ORIGIN", "SCALE", "ROT", "IS",	  // 保留字（一字一码）
	"TO", "STEP", "DRAW","FOR", "FROM", // 保留字
	"T",				  // 参数
	"SEMICO", "L_BRACKET", "R_BRACKET", "COMMA",// 分隔符
	"PLUS", "MINUS", "MUL", "DIV", "POWER",		// 运算符
	"FUNC",				  // 函数（调用）
	"CONST_ID",			  // 常数
	"NONTOKEN",			  // 空记号（源程序结束）
	"ERRTOKEN"			  // 出错记号（非法输入）
};


//-----------------------格式化打印token
void printToken(Token token);

//-----------------------初始化词法分析器
ScannerErrCode initScanner(const char* fileName);

//-----------------------关闭词法分析器
void closeScanner();

//-----------------------获取一个语法记号
Token getToken();

//-----------------------scanner的测试函数
void scanner(const char* fileName);
