#include "stdafx.h"
extern HDC hDC;
//------------------------计算表达式的值
double getExprValue(struct ExprNode * root)
{
	if (root == NULL) return 0.0;
	switch (root->opCode)
	{
	case PLUS:
		return getExprValue(root->content.caseOperator.left) +
			getExprValue(root->content.caseOperator.right);
	case MINUS:
		return getExprValue(root->content.caseOperator.left) -
			getExprValue(root->content.caseOperator.right);
	case MUL:
		return getExprValue(root->content.caseOperator.left) *
			getExprValue(root->content.caseOperator.right);
	case DIV:
		return getExprValue(root->content.caseOperator.left) /
			getExprValue(root->content.caseOperator.right);
	case POWER:
		return pow(getExprValue(root->content.caseOperator.left),
			getExprValue(root->content.caseOperator.right));
	case FUNC:
		return (*root->content.caseFunc.mathFuncPtr)
			(getExprValue(root->content.caseFunc.child));
	case CONST_ID:
		return root->content.caseConst;
	case T:
		return *(root->content.caseParamPtr);
	default:
		return 0.0;
	}
}

//------------------------计算被绘制点的坐标
static void calcCoord(struct ExprNode *horExp, struct ExprNode *verExp, double *horX, double *verY)
{
	double horCord, verCord, horTmp;

	// 计算表达式的值，得到点的原始坐标
	horCord = getExprValue(horExp);
	verCord = getExprValue(verExp);

	// 进行比例变换
	horCord *= scaleX;
	verCord *= scaleY;

	// 进行旋转变换
	horTmp = horCord * cos(rotAngle) + verCord * sin(rotAngle);
	verCord = verCord * cos(rotAngle) - horCord * sin(rotAngle);
	horCord = horTmp;

	// 进行平移变换
	horCord += originX;
	verCord += originY;

	// 返回变换后点的坐标
	*horX = horCord;
	*verY = verCord;
}

//------------------------绘制像素点
static void drawPixel(unsigned long x, unsigned long y)
{
	COLORREF red = RGB(255,0,0);
	SetPixel(hDC, x, y,red);
	SetPixel(hDC, x + 1, y, red);
	SetPixel(hDC, x, y + 1, red);
	SetPixel(hDC, x + 1, y + 1, red);
}

/**
 * Param:(
 * start   起点
 * end     终点
 * step    步长
 * horPtr  横轴语法树
 * verPtr  纵轴语法树
 * )
 * Return: void
 * 循环绘制点坐标
**/
void drawLoop(double start, double end, double step, struct ExprNode * horPtr, struct ExprNode * verPtr)
{
	extern double parameter;
	double x, y;
	for (parameter = start; parameter <= end; parameter += step)
	{
		calcCoord(horPtr, verPtr, &x, &y);
		drawPixel((unsigned long)x, (unsigned long)y);
	}
}

//------------------------删除一棵语法树
void delExprTree(struct ExprNode * root)
{
	if (root == NULL) return;
	switch (root->opCode)
	{
	case PLUS:			// 两个孩子的内部节点
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		delExprTree(root->content.caseOperator.left);
		delExprTree(root->content.caseOperator.right);
		break;
	case FUNC: 			// 一个孩子的内部节点
		delExprTree(root->content.caseFunc.child);
		break;
	default: 			// 叶子节点
		break;
	}
	free(root);				// 删除叶子节点
}