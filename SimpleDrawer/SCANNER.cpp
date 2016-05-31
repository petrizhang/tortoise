#include "stdafx.h"

#define SETBUFFERVALUE(ch) if((errno=setBufferValue(ch))!=SCANNER_OK) return raiseERRTOKEN((enum ScannerErrCode) errno)

unsigned int lineNo=1;				        // 跟踪源文件行号
static FILE *inFile=NULL;				        // 输入文件流

static FILE *tokenFile = NULL;				        // 输出符号流的文件流
static const char *tokenFilePath = "./output/tokens.txt";  //输出符号流的文件路径

const char* syntaxTreeFilePath = "./output/SyntaxTree.txt";//打印语法树的文件路径
FILE* syntaxTreeFile = NULL;// 打印语法树的文件流

const char* errorFilePath = "./output/errors.txt";//打印错误的文件路径
FILE* errorFile = NULL;// 打印错误的文件流

static char tokenBuffer[MAX_TOKEN_SIZE + 1];	// 记号字符缓冲
static int validBufferLength;					// 记录已写入缓冲区的长度

//-----------------------格式化打印token
void printToken(Token token)
{
#ifdef DEBUG
	if (token.FUNC_PTR == NULL) {
		fprintf_s(tokenFile, "%10s%10s%15.4f  %10s%10d\n", token_repr[token.token_type], token.token_string, token.value, "null", lineNo);
	}
	else {
		fprintf_s(tokenFile, "%10s%10s%15.4f  0x%08X%10d\n", token_repr[token.token_type], token.token_string, token.value, token.FUNC_PTR, lineNo);
	}
#endif
}

//-----------------------返回一个错误token
Token static raiseERRTOKEN(ScannerErrCode code)
{
	char* info = "unknown error";
	switch (code)
	{
	case TOKEN_OVERFLOW:
		info = "token's size is too larger(larger than 100)";
		break;
	case INVALID_TOKEN:
		info = "invalid token";
		break;
	case OPEN_FILE_FAILED:
		info = "failed to open file";
	default:
		break;
	}
	char msg[256];
	sprintf_s(msg,"SyntaxError in line %d near \"%s\": %s\n", lineNo, tokenBuffer, info);
	MessageBox(NULL, msg, "错误", MB_OK);
	fprintf_s(errorFile, "SyntaxError in line %d near \"%s\": %s\n", lineNo, tokenBuffer, info);
	return{ ERRTOKEN,tokenBuffer,0,NULL };
}
//-----------------------从输入文件读入一个字符
char static getChar()
{
	char ch = getc(inFile);
	return toupper(ch);
}

//-----------------------将一个字符退回到输入文件中
void static  backChar(char ch)
{
	if (ch != EOF) {
		ungetc(ch, inFile);
	}
}

//-----------------------清空tokenBuffer(简单地将curTokenLength的值设为0)
void static setBufferEmpty()
{
	tokenBuffer[0] = '\0';
	validBufferLength = 0;
}

//-----------------------将tokenBuffer指定位置的值设置为ch
ScannerErrCode static setBufferValue(char ch)
{
	if (validBufferLength > MAX_TOKEN_SIZE) {
		tokenBuffer[MAX_TOKEN_SIZE] = '\0';
		return TOKEN_OVERFLOW;
	}
	tokenBuffer[validBufferLength] = ch;
	validBufferLength++;
	return SCANNER_OK;
}

//-----------------------在ID表中查找ID代表的token
Token static  lookupId(const char* idString)
{
	int i;
	for (i = 0; i < ID_TAB_TOTALS; i++) {
		if (strcmp(idString, id_tab[i].token_string) == 0) {
			return id_tab[i];
		}
	}
	return raiseERRTOKEN(INVALID_TOKEN);
}

//-----------------------初始化词法分析器
ScannerErrCode initScanner(const char* fileName)
{
	char msg[256];

	if (fopen_s(&errorFile, errorFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", errorFilePath);
		MessageBox(NULL, msg, "错误", MB_OK);
		exit(1);
	}

	if (fopen_s(&inFile, fileName, "r") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", fileName);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", fileName);
		MessageBox(NULL, msg, "错误", MB_OK);
		exit(1);
	}

	if (fopen_s(&syntaxTreeFile, syntaxTreeFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", syntaxTreeFilePath);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", syntaxTreeFilePath);
		MessageBox(NULL, msg, "错误", MB_OK);
		exit(1);
	}

	return SCANNER_OK;
}

//-----------------------关闭词法分析器
void closeScanner()
{
	if (inFile != NULL) {
		fclose(inFile);
	}
	if (syntaxTreeFile != NULL) {
		fclose(syntaxTreeFile);
	}
	if (errorFile != NULL) {
		fclose(errorFile);
	}
	lineNo = 1;
	setBufferEmpty();
}

//-----------------------获取一个语法记号
Token getToken()
{
	if (inFile == NULL) {
		return raiseERRTOKEN(OPEN_FILE_FAILED);
	}
	Token token;
	memset(&token, 0, sizeof(token));
	token.token_string = tokenBuffer;
	//首先清空缓冲区
	setBufferEmpty();
	//读取一个字符
	char ch = getChar();
	if (ch == EOF) {
		token.token_type = NONTOKEN;
		return token;
	}
	//略过空白字符
	while (isspace(ch)) {
		if (ch == '\n') {
			lineNo++;
		}
		ch = getChar();
	}
	if (ch == EOF) {
		token.token_type = NONTOKEN;
		return token;
	}
	//逐个读取字符进行匹配并写入缓冲区
	if (isalpha(ch)) {             //如果是字母
		SETBUFFERVALUE(ch);
		ch = getChar();
		while (isalpha(ch)) {
			SETBUFFERVALUE(ch);
			ch = getChar();
		}
		backChar(ch);
		SETBUFFERVALUE('\0');
		token = lookupId(tokenBuffer);
		token.token_string = tokenBuffer;
	}
	else if (isdigit(ch)) {        //如果是数字
		while (isdigit(ch)) {
			SETBUFFERVALUE(ch);
			ch = getChar();
		}
		if (ch == '.') {
			SETBUFFERVALUE(ch);
			ch = getChar();
			if (isdigit(ch)) {
				while (isdigit(ch)) {
					SETBUFFERVALUE(ch);
					ch = getChar();
				}
			}
			else {//如果小数点后面不是数字，那么一定是非法输入
				return raiseERRTOKEN(INVALID_TOKEN);
			}
		}
		backChar(ch);
		SETBUFFERVALUE('\0');
		token.token_type = CONST_ID;
		token.value = strtod(tokenBuffer, NULL);
	}
	else {//既不是数字也不是字母，那么一定是符号或者违法字符
		SETBUFFERVALUE(ch);
		switch (ch) {
		case ';':
			SETBUFFERVALUE('\0');
			token.token_type = SEMICO;
			break;
		case '(':
			SETBUFFERVALUE('\0');
			token.token_type = L_BRACKET;
			break;
		case ')':
			SETBUFFERVALUE('\0');
			token.token_type = R_BRACKET;
			break;
		case ',':
			token.token_type = COMMA;
			break;
		case '+':
			SETBUFFERVALUE('\0');
			token.token_type = PLUS;
			break;
		case '-':
			ch = getChar();
			if (ch == '-') {//遇到注释符号，略过所有字符直到行尾或者文件结束
				while (ch != '\n' && ch != EOF) {
					ch = getChar();
				}
				backChar(ch);
				return getToken();
			}
			else {
				backChar(ch);
				SETBUFFERVALUE('\0');
				token.token_type = MINUS;
				break;
			}
		case '/':
			ch = getChar();
			if (ch == '/') {
				while (ch != '\n' && ch != EOF) {
					ch = getChar();
				}
				backChar(ch);
				return getToken();
			}
			else
			{
				backChar(ch);
				SETBUFFERVALUE('\0');
				token.token_type = DIV;
				break;
			}
		case '*':
			ch = getChar();
			if (ch == '*') {
				SETBUFFERVALUE('*');
				SETBUFFERVALUE('\0');
				token.token_type = POWER;
				break;
			}
			else {
				backChar(ch);
				SETBUFFERVALUE('\0');
				token.token_type = MUL;
				break;
			}
		default:
			return raiseERRTOKEN(INVALID_TOKEN);
			break;
		} // end of switch
	}
	return token;
}

//-----------------------scanner的测试函数
void scanner(const char* fileName) {
	char msg[256];
	if (fopen_s(&tokenFile, tokenFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", tokenFilePath);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", tokenFilePath);
		MessageBox(NULL, msg, "错误", MB_OK);
		exit(1);
	}

	initScanner(fileName);
	Token token = getToken();
	while (token.token_type != NONTOKEN) {
		printToken(token);
		token = getToken();
	}
	printToken(token);
	closeScanner();

	if (tokenFile != NULL) {
		fclose(tokenFile);
	}
}