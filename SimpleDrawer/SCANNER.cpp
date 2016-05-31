#include "stdafx.h"

#define SETBUFFERVALUE(ch) if((errno=setBufferValue(ch))!=SCANNER_OK) return raiseERRTOKEN((enum ScannerErrCode) errno)

unsigned int lineNo=1;				        // ����Դ�ļ��к�
static FILE *inFile=NULL;				        // �����ļ���

static FILE *tokenFile = NULL;				        // ������������ļ���
static const char *tokenFilePath = "./output/tokens.txt";  //������������ļ�·��

const char* syntaxTreeFilePath = "./output/SyntaxTree.txt";//��ӡ�﷨�����ļ�·��
FILE* syntaxTreeFile = NULL;// ��ӡ�﷨�����ļ���

const char* errorFilePath = "./output/errors.txt";//��ӡ������ļ�·��
FILE* errorFile = NULL;// ��ӡ������ļ���

static char tokenBuffer[MAX_TOKEN_SIZE + 1];	// �Ǻ��ַ�����
static int validBufferLength;					// ��¼��д�뻺�����ĳ���

//-----------------------��ʽ����ӡtoken
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

//-----------------------����һ������token
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
	MessageBox(NULL, msg, "����", MB_OK);
	fprintf_s(errorFile, "SyntaxError in line %d near \"%s\": %s\n", lineNo, tokenBuffer, info);
	return{ ERRTOKEN,tokenBuffer,0,NULL };
}
//-----------------------�������ļ�����һ���ַ�
char static getChar()
{
	char ch = getc(inFile);
	return toupper(ch);
}

//-----------------------��һ���ַ��˻ص������ļ���
void static  backChar(char ch)
{
	if (ch != EOF) {
		ungetc(ch, inFile);
	}
}

//-----------------------���tokenBuffer(�򵥵ؽ�curTokenLength��ֵ��Ϊ0)
void static setBufferEmpty()
{
	tokenBuffer[0] = '\0';
	validBufferLength = 0;
}

//-----------------------��tokenBufferָ��λ�õ�ֵ����Ϊch
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

//-----------------------��ID���в���ID�����token
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

//-----------------------��ʼ���ʷ�������
ScannerErrCode initScanner(const char* fileName)
{
	char msg[256];

	if (fopen_s(&errorFile, errorFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", errorFilePath);
		MessageBox(NULL, msg, "����", MB_OK);
		exit(1);
	}

	if (fopen_s(&inFile, fileName, "r") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", fileName);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", fileName);
		MessageBox(NULL, msg, "����", MB_OK);
		exit(1);
	}

	if (fopen_s(&syntaxTreeFile, syntaxTreeFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", syntaxTreeFilePath);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", syntaxTreeFilePath);
		MessageBox(NULL, msg, "����", MB_OK);
		exit(1);
	}

	return SCANNER_OK;
}

//-----------------------�رմʷ�������
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

//-----------------------��ȡһ���﷨�Ǻ�
Token getToken()
{
	if (inFile == NULL) {
		return raiseERRTOKEN(OPEN_FILE_FAILED);
	}
	Token token;
	memset(&token, 0, sizeof(token));
	token.token_string = tokenBuffer;
	//������ջ�����
	setBufferEmpty();
	//��ȡһ���ַ�
	char ch = getChar();
	if (ch == EOF) {
		token.token_type = NONTOKEN;
		return token;
	}
	//�Թ��հ��ַ�
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
	//�����ȡ�ַ�����ƥ�䲢д�뻺����
	if (isalpha(ch)) {             //�������ĸ
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
	else if (isdigit(ch)) {        //���������
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
			else {//���С������治�����֣���ôһ���ǷǷ�����
				return raiseERRTOKEN(INVALID_TOKEN);
			}
		}
		backChar(ch);
		SETBUFFERVALUE('\0');
		token.token_type = CONST_ID;
		token.value = strtod(tokenBuffer, NULL);
	}
	else {//�Ȳ�������Ҳ������ĸ����ôһ���Ƿ��Ż���Υ���ַ�
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
			if (ch == '-') {//����ע�ͷ��ţ��Թ������ַ�ֱ����β�����ļ�����
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

//-----------------------scanner�Ĳ��Ժ���
void scanner(const char* fileName) {
	char msg[256];
	if (fopen_s(&tokenFile, tokenFilePath, "w") != 0) {
		sprintf_s(msg, "Error in initScanner: Failed to open file %s\n", tokenFilePath);
		fprintf_s(errorFile, "Error in initScanner: Failed to open file %s\n", tokenFilePath);
		MessageBox(NULL, msg, "����", MB_OK);
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