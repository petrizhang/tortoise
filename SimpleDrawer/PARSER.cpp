#include "stdafx.h"

extern const char* syntaxTreeFilePath;//打印语法树的文件路径
extern FILE* syntaxTreeFile ;// 打印语法树的文件流

extern const char* errorFilePath ;//打印错误的文件路径
extern FILE* errorFile;// 打印错误的文件流

static Token lookAhead;
static enum TokenType* lookAheadType = &lookAhead.token_type;//设一指针维持对lookAhead.tokenType的引用,以降低访问时间

double parameter = 0,               // 参数T的存储空间
originX = 0, originY = 0,			// 横、纵平移距离
scaleX = 1, scaleY = 1,     	    // 横、纵比例因子
rotAngle = 0;					    // 旋转角度

//-----------------------递归子程序的函数声明
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
 * 接受可变参数，根据opCode（记号类型）建立一个结点
 * 参数形式有如下几种：
 * (TokenType T);
 * (TokenType CONST_ID, double value);
 * (TokenType FUNC, FuncPtr pointer, ExprNode* child);
 * (TokenType others, ExprNode* left, ExprNode* right);
**/
static ExprNode* makeNode(enum TokenType opCode ...)
{
	ExprNode* node = (ExprNode *)malloc(sizeof(struct ExprNode));
	va_list ap;//可变参数列表

	node->opCode = opCode;
	switch (opCode)
	{
	case CONST_ID://常量结点
		va_start(ap, opCode);
		node->content.caseConst = va_arg(ap, double);//取一个double类型的参数
		va_end(ap);
		break;
	case T://变量结点
		node->content.caseParamPtr = &parameter;
		break;
	case FUNC://函数结点
		va_start(ap, opCode);
		node->content.caseFunc.mathFuncPtr = va_arg(ap, FuncPtr);//取一个FuncPtr类型的参数
		node->content.caseFunc.child = va_arg(ap, ExprNode*);//取一个ExprNode*类型的参数
		va_end(ap);
		break;
	default://二元运算结点
		va_start(ap, opCode);
		node->content.caseOperator.left = va_arg(ap, ExprNode*);//取一个ExprNode*类型的参数
		node->content.caseOperator.right = va_arg(ap, ExprNode*);//取一个ExprNode*类型的参数
		va_end(ap);
		break;
	}
	return node;
}

#ifdef DEBUG

#define QUEUE_SIZE 65535
typedef struct Element {
	ExprNode* node;
	int level;//结点所处层次
	int parentPos;//父结点在其层中的位置
} Element;

static Element queue[QUEUE_SIZE];
static Element* spaceEnd = queue + QUEUE_SIZE;
static Element* start = queue;
static Element* end = queue;

//-----------------------从queue的队头取元素并删除
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
//-----------------------向queue的队尾加元素
static void add(Element e)
{
	*end = e;
	end++;
	if (end == spaceEnd) {
		end = queue;
	}
	if (end + 1 == start) {
		printf("队列溢出：语法树的结点过多");
		exit(OVERFLOW);
	}
}

//-----------------------判断queue是否为空
BOOL isEmpty()
{
	if (start == end) {
		return 1;
	}
	return 0;
}

//-----------------------向文件file以及屏幕输出n个空格
void printSpace(FILE* file, int n)
{
	for (int i = 1; i < n; i++) {
		printf(" ");
		fprintf_s(file, " ");
	}
}

//-----------------------广度优先遍历打印语法树
static void printSyntaxTree(ExprNode* root, int levelInterval)
{
	FILE* file = syntaxTreeFile;
	if (file == NULL) {
		exit(errno);
	}
	Element e;//从队列中取出的元素
	ExprNode* nodePtr;
	int preLevel = 0, curLevel = 1;//上一次/这一次打印时，打印进程所在的层次
	int pos = 0;//结点在本层中的位置
	int firstFlag = 0;//标志结点是否是本层第一个结点
	add({ root,1,0 });//将根结点加入队列
	while (!isEmpty()) {//当队列非空，一直遍历
		firstFlag = 0;
		e = pop();//取元素
		nodePtr = e.node;//从元素取结点
		curLevel = e.level;//从元素取结点层次
		if (curLevel != preLevel) {
			levelInterval /= 2;//计算打印间隔
			firstFlag = 1;
			putchar('\n');
			fprintf_s(file, "\n");
		}

		//打印结点之前先打印一定量的空格
		printSpace(file, (firstFlag ? levelInterval / 2 : levelInterval) - 5);

		switch (nodePtr->opCode)
		{
		case CONST_ID://常量
			printf("%5.2f", nodePtr->content.caseConst);
			fprintf_s(file, "%5.2f", nodePtr->content.caseConst);
			break;
		case T://变量
			putchar('T');
			fprintf_s(file, "T");
			break;
		case FUNC://函数调用
			printf("FUNC ");
			fprintf_s(file, "FUNC ");
			add({ nodePtr->content.caseFunc.child,curLevel + 1 });
			break;
		default://二元运算
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

//-----------------------向控制台输出出错信息
void raiseSyntaxError(const char* realToken, const char* expectToken)
{
	char msg[256] ;
	sprintf_s(msg, "SyntaxError in line %d near \"%s\": expect %s but get %s\n", lineNo, lookAhead.token_string, expectToken, realToken);
	fprintf_s(errorFile, "SyntaxError in line %d near \"%s\": expect %s but get %s\n", lineNo, lookAhead.token_string, expectToken, realToken);
	MessageBox(NULL, msg, "错误", MB_OK);
}

//-----------------------从输入文件读入一个记号
static void fetchToken()
{
	lookAhead = getToken();//错误记号的话getToken会打印出错误信息
}

//-----------------------匹配一种token，匹配成功继续读入下一个，匹配失败报语法错误
static void matchToken(enum TokenType type)
{
	if (*lookAheadType == type) {
		fetchToken();
	}
	else {//进行错误恢复
		raiseSyntaxError(lookAhead.token_string, token_repr[type]);
		fetchToken();
	}
}

//-----------------------递归子程序program
static void program()
{
	while (*lookAheadType != NONTOKEN) {
		statement();
		matchToken(SEMICO);
	}
}

//-----------------------递归子程序statement
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

//-----------------------递归子程序originStatement
static void originStatement()
{
	ExprNode *originXPtr, *originYPtr;
	matchToken(ORIGIN);
	matchToken(IS);
	matchToken(L_BRACKET);  originXPtr = expression();  originX = getExprValue(originXPtr);
	matchToken(COMMA);      originYPtr = expression();  originY = getExprValue(originYPtr);
	matchToken(R_BRACKET);
}

//-----------------------递归子程序scaleStatement
static void scaleStatement()
{
	ExprNode *scaleXPtr, *scaleYPtr;
	matchToken(SCALE);
	matchToken(IS);
	matchToken(L_BRACKET);  scaleXPtr = expression();  scaleX = getExprValue(scaleXPtr);  delExprTree(scaleXPtr);
	matchToken(COMMA);      scaleYPtr = expression();  scaleY = getExprValue(scaleYPtr);  delExprTree(scaleYPtr);
	matchToken(R_BRACKET);
}

//-----------------------递归子程序rotStatement
static void rotStatement()
{
	ExprNode *rotPtr;
	matchToken(ROT);
	matchToken(IS);
	rotPtr = expression();  rotAngle = getExprValue(rotPtr);  delExprTree(rotPtr);
}

//-----------------------递归子程序forStatement
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

//-----------------------递归子程序expression
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

//-----------------------递归子程序term
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

//-----------------------递归子程序factor
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

//-----------------------递归子程序component
static struct ExprNode*  component()
{
	ExprNode* nodePtr = NULL;
	ExprNode* left = NULL;
	ExprNode* right = NULL;
	left = atom();
	if (*lookAheadType == POWER) {//如果有匹配到POWER运算符，那么构造一个二元运算结点
		matchToken(POWER);
		right = component();
		return makeNode(POWER, left, right);
	}
	else {//否则直接返回atom()返回的结点
		return left;
	}
}

//-----------------------递归子程序atom
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

//-----------------------parser的测试函数
void parser(const char* fileName)
{
	initScanner(fileName);
	fetchToken();
	program();
	closeScanner();
}
