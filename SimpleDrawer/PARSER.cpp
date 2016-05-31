#include "stdafx.h"

extern const char* syntaxTreeFilePath;//��ӡ�﷨�����ļ�·��
extern FILE* syntaxTreeFile ;// ��ӡ�﷨�����ļ���

extern const char* errorFilePath ;//��ӡ������ļ�·��
extern FILE* errorFile;// ��ӡ������ļ���

static Token lookAhead;
static enum TokenType* lookAheadType = &lookAhead.token_type;//��һָ��ά�ֶ�lookAhead.tokenType������,�Խ��ͷ���ʱ��

double parameter = 0,               // ����T�Ĵ洢�ռ�
originX = 0, originY = 0,			// �ᡢ��ƽ�ƾ���
scaleX = 1, scaleY = 1,     	    // �ᡢ�ݱ�������
rotAngle = 0;					    // ��ת�Ƕ�

//-----------------------�ݹ��ӳ���ĺ�������
static void program();
static void statement();
static void originStatement();
static void scaleStatement();
static void rotStatement();
static void forStatement();
static struct ExprNode* expression();
static struct ExprNode* term();
static struct ExprNode* factor();
static struct ExprNode* component();
static struct ExprNode* atom();

/**
 * ���ܿɱ����������opCode���Ǻ����ͣ�����һ�����
 * ������ʽ�����¼��֣�
 * (TokenType T);
 * (TokenType CONST_ID, double value);
 * (TokenType FUNC, FuncPtr pointer, ExprNode* child);
 * (TokenType others, ExprNode* left, ExprNode* right);
**/
static ExprNode* makeNode(enum TokenType opCode ...)
{
	ExprNode* node = (ExprNode *)malloc(sizeof(struct ExprNode));
	va_list ap;//�ɱ�����б�

	node->opCode = opCode;
	switch (opCode)
	{
	case CONST_ID://�������
		va_start(ap, opCode);
		node->content.caseConst = va_arg(ap, double);//ȡһ��double���͵Ĳ���
		va_end(ap);
		break;
	case T://�������
		node->content.caseParamPtr = &parameter;
		break;
	case FUNC://�������
		va_start(ap, opCode);
		node->content.caseFunc.mathFuncPtr = va_arg(ap, FuncPtr);//ȡһ��FuncPtr���͵Ĳ���
		node->content.caseFunc.child = va_arg(ap, ExprNode*);//ȡһ��ExprNode*���͵Ĳ���
		va_end(ap);
		break;
	default://��Ԫ������
		va_start(ap, opCode);
		node->content.caseOperator.left = va_arg(ap, ExprNode*);//ȡһ��ExprNode*���͵Ĳ���
		node->content.caseOperator.right = va_arg(ap, ExprNode*);//ȡһ��ExprNode*���͵Ĳ���
		va_end(ap);
		break;
	}
	return node;
}

#ifdef DEBUG

#define QUEUE_SIZE 65535
typedef struct Element {
	ExprNode* node;
	int level;//����������
	int parentPos;//�����������е�λ��
} Element;

static Element queue[QUEUE_SIZE];
static Element* spaceEnd = queue + QUEUE_SIZE;
static Element* start = queue;
static Element* end = queue;

//-----------------------��queue�Ķ�ͷȡԪ�ز�ɾ��
static Element pop()
{
	Element e;
	e = *start;
	start++;
	if (start == spaceEnd) {
		start = queue;
	}
	return e;
}
//-----------------------��queue�Ķ�β��Ԫ��
static void add(Element e)
{
	*end = e;
	end++;
	if (end == spaceEnd) {
		end = queue;
	}
	if (end + 1 == start) {
		printf("����������﷨���Ľ�����");
		exit(OVERFLOW);
	}
}

//-----------------------�ж�queue�Ƿ�Ϊ��
BOOL isEmpty()
{
	if (start == end) {
		return 1;
	}
	return 0;
}

//-----------------------���ļ�file�Լ���Ļ���n���ո�
void printSpace(FILE* file, int n)
{
	for (int i = 1; i < n; i++) {
		printf(" ");
		fprintf_s(file, " ");
	}
}

//-----------------------������ȱ�����ӡ�﷨��
static void printSyntaxTree(ExprNode* root, int levelInterval)
{
	FILE* file = syntaxTreeFile;
	if (file == NULL) {
		exit(errno);
	}
	Element e;//�Ӷ�����ȡ����Ԫ��
	ExprNode* nodePtr;
	int preLevel = 0, curLevel = 1;//��һ��/��һ�δ�ӡʱ����ӡ�������ڵĲ��
	int pos = 0;//����ڱ����е�λ��
	int firstFlag = 0;//��־����Ƿ��Ǳ����һ�����
	add({ root,1,0 });//�������������
	while (!isEmpty()) {//�����зǿգ�һֱ����
		firstFlag = 0;
		e = pop();//ȡԪ��
		nodePtr = e.node;//��Ԫ��ȡ���
		curLevel = e.level;//��Ԫ��ȡ�����
		if (curLevel != preLevel) {
			levelInterval /= 2;//�����ӡ���
			firstFlag = 1;
			putchar('\n');
			fprintf_s(file, "\n");
		}

		//��ӡ���֮ǰ�ȴ�ӡһ�����Ŀո�
		printSpace(file, (firstFlag ? levelInterval / 2 : levelInterval) - 5);

		switch (nodePtr->opCode)
		{
		case CONST_ID://����
			printf("%5.2f", nodePtr->content.caseConst);
			fprintf_s(file, "%5.2f", nodePtr->content.caseConst);
			break;
		case T://����
			putchar('T');
			fprintf_s(file, "T");
			break;
		case FUNC://��������
			printf("FUNC ");
			fprintf_s(file, "FUNC ");
			add({ nodePtr->content.caseFunc.child,curLevel + 1 });
			break;
		default://��Ԫ����
			printf("%5s", token_repr[nodePtr->opCode]);
			fprintf_s(file, "%5s", token_repr[nodePtr->opCode]);
			add({ nodePtr->content.caseOperator.left, curLevel + 1 });
			add({ nodePtr->content.caseOperator.right, curLevel + 1 });
			break;
		}
		preLevel = curLevel;
	}
	putchar('\n');
	fprintf_s(file, "\n\n\n");
}
#endif

//-----------------------�����̨���������Ϣ
void raiseSyntaxError(const char* realToken, const char* expectToken)
{
	char msg[256] ;
	sprintf_s(msg, "SyntaxError in line %d near \"%s\": expect %s but get %s\n", lineNo, lookAhead.token_string, expectToken, realToken);
	fprintf_s(errorFile, "SyntaxError in line %d near \"%s\": expect %s but get %s\n", lineNo, lookAhead.token_string, expectToken, realToken);
	MessageBox(NULL, msg, "����", MB_OK);
}

//-----------------------�������ļ�����һ���Ǻ�
static void fetchToken()
{
	lookAhead = getToken();//����ǺŵĻ�getToken���ӡ��������Ϣ
}

//-----------------------ƥ��һ��token��ƥ��ɹ�����������һ����ƥ��ʧ�ܱ��﷨����
static void matchToken(enum TokenType type)
{
	if (*lookAheadType == type) {
		fetchToken();
	}
	else {//���д���ָ�
		raiseSyntaxError(lookAhead.token_string, token_repr[type]);
		fetchToken();
	}
}

//-----------------------�ݹ��ӳ���program
static void program()
{
	while (*lookAheadType != NONTOKEN) {
		statement();
		matchToken(SEMICO);
	}
}

//-----------------------�ݹ��ӳ���statement
static void statement()
{
	switch (*lookAheadType)
	{
	case ORIGIN:
		originStatement();
		break;
	case SCALE:
		scaleStatement();
		break;
	case ROT:
		rotStatement();
		break;
	case FOR:
		forStatement();
		break;
	default:
		raiseSyntaxError(lookAhead.token_string, "ORIGIN/SCALE/ROT/FOR");
		break;
	}
}

//-----------------------�ݹ��ӳ���originStatement
static void originStatement()
{
	ExprNode *originXPtr, *originYPtr;
	matchToken(ORIGIN);
	matchToken(IS);
	matchToken(L_BRACKET);  originXPtr = expression();  originX = getExprValue(originXPtr);
	matchToken(COMMA);      originYPtr = expression();  originY = getExprValue(originYPtr);
	matchToken(R_BRACKET);
}

//-----------------------�ݹ��ӳ���scaleStatement
static void scaleStatement()
{
	ExprNode *scaleXPtr, *scaleYPtr;
	matchToken(SCALE);
	matchToken(IS);
	matchToken(L_BRACKET);  scaleXPtr = expression();  scaleX = getExprValue(scaleXPtr);  delExprTree(scaleXPtr);
	matchToken(COMMA);      scaleYPtr = expression();  scaleY = getExprValue(scaleYPtr);  delExprTree(scaleYPtr);
	matchToken(R_BRACKET);
}

//-----------------------�ݹ��ӳ���rotStatement
static void rotStatement()
{
	ExprNode *rotPtr;
	matchToken(ROT);
	matchToken(IS);
	rotPtr = expression();  rotAngle = getExprValue(rotPtr);  delExprTree(rotPtr);
}

//-----------------------�ݹ��ӳ���forStatement
static void forStatement()
{
	struct ExprNode *startPtr, *endPtr, *stepPtr, *xPtr, *yPtr;
	double start, end, step;
	matchToken(FOR);
	matchToken(T);
	matchToken(FROM);       startPtr = expression();  start = getExprValue(startPtr);  delExprTree(startPtr);
	matchToken(TO);	          endPtr = expression();    end = getExprValue(endPtr);    delExprTree(endPtr);
	matchToken(STEP);        stepPtr = expression();   step = getExprValue(stepPtr);   delExprTree(stepPtr);
	matchToken(DRAW);
	matchToken(L_BRACKET);	xPtr = expression(); 

	#ifdef DEBUG
	printSyntaxTree(xPtr, 256);
	#endif

	matchToken(COMMA);	    yPtr = expression();
	matchToken(R_BRACKET);
	drawLoop(start, end, step, xPtr, yPtr);
	delExprTree(xPtr);
	delExprTree(yPtr);
}

//-----------------------�ݹ��ӳ���expression
static struct ExprNode* expression()
{
	ExprNode* left = NULL;
	ExprNode* right = NULL;
	enum TokenType tmpTokenType;
	left = term();
	while (*lookAheadType == PLUS || *lookAheadType == MINUS) {
		tmpTokenType = *lookAheadType;
		matchToken(*lookAheadType);
		right = term();
		left = makeNode(tmpTokenType, left, right);
	}
	return left;
}

//-----------------------�ݹ��ӳ���term
static struct ExprNode*  term()
{
	ExprNode* left = NULL;
	ExprNode* right = NULL;
	enum TokenType tmpTokenType;
	left = factor();
	while (*lookAheadType == MUL || *lookAheadType == DIV) {
		tmpTokenType = *lookAheadType;
		matchToken(*lookAheadType);
		right = factor();
		left = makeNode(tmpTokenType, left, right);
	}
	return left;
}

//-----------------------�ݹ��ӳ���factor
static struct ExprNode*  factor()
{
	ExprNode* nodePtr = NULL;
	ExprNode* left = makeNode(CONST_ID, 0.0);
	ExprNode* right = NULL;
	switch (*lookAheadType)
	{
	case PLUS:
		matchToken(PLUS);
		right = factor();
		nodePtr = makeNode(PLUS, left, right);
		break;
	case MINUS:
		matchToken(MINUS);
		right = factor();
		nodePtr = makeNode(MINUS, left, right);
		break;
	default:
		nodePtr = component();
		break;
	}
	return nodePtr;
}

//-----------------------�ݹ��ӳ���component
static struct ExprNode*  component()
{
	ExprNode* nodePtr = NULL;
	ExprNode* left = NULL;
	ExprNode* right = NULL;
	left = atom();
	if (*lookAheadType == POWER) {//�����ƥ�䵽POWER���������ô����һ����Ԫ������
		matchToken(POWER);
		right = component();
		return makeNode(POWER, left, right);
	}
	else {//����ֱ�ӷ���atom()���صĽ��
		return left;
	}
}

//-----------------------�ݹ��ӳ���atom
static struct ExprNode* atom()
{
	Token tmpToken = lookAhead;
	struct ExprNode* nodePtr = NULL;
	struct ExprNode* childPtr = NULL;
	switch (*lookAheadType)
	{
	case CONST_ID:
		matchToken(CONST_ID);
		nodePtr = makeNode(CONST_ID, tmpToken.value);
		break;
	case T:
		matchToken(T);
		nodePtr = makeNode(T);
		break;
	case FUNC:
		matchToken(FUNC);
		matchToken(L_BRACKET);
		childPtr = expression();
		nodePtr = makeNode(FUNC, tmpToken.FUNC_PTR, childPtr);
		matchToken(R_BRACKET);
		break;
	case L_BRACKET:
		matchToken(L_BRACKET);
		nodePtr = expression();
		matchToken(R_BRACKET);
		break;
	default:
		raiseSyntaxError(tmpToken.token_string, "CONST_ID/T/FUNC/L_BRAKCKET");
		break;
	}
	return nodePtr;
}

//-----------------------parser�Ĳ��Ժ���
void parser(const char* fileName)
{
	initScanner(fileName);
	fetchToken();
	program();
	closeScanner();
}
