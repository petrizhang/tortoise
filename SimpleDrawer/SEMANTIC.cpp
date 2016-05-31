#include "stdafx.h"
extern HDC hDC;
//------------------------������ʽ��ֵ
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

//------------------------���㱻���Ƶ������
static void calcCoord(struct ExprNode *horExp, struct ExprNode *verExp, double *horX, double *verY)
{
	double horCord, verCord, horTmp;

	// ������ʽ��ֵ���õ����ԭʼ����
	horCord = getExprValue(horExp);
	verCord = getExprValue(verExp);

	// ���б����任
	horCord *= scaleX;
	verCord *= scaleY;

	// ������ת�任
	horTmp = horCord * cos(rotAngle) + verCord * sin(rotAngle);
	verCord = verCord * cos(rotAngle) - horCord * sin(rotAngle);
	horCord = horTmp;

	// ����ƽ�Ʊ任
	horCord += originX;
	verCord += originY;

	// ���ر任��������
	*horX = horCord;
	*verY = verCord;
}

//------------------------�������ص�
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
 * start   ���
 * end     �յ�
 * step    ����
 * horPtr  �����﷨��
 * verPtr  �����﷨��
 * )
 * Return: void
 * ѭ�����Ƶ�����
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

//------------------------ɾ��һ���﷨��
void delExprTree(struct ExprNode * root)
{
	if (root == NULL) return;
	switch (root->opCode)
	{
	case PLUS:			// �������ӵ��ڲ��ڵ�
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		delExprTree(root->content.caseOperator.left);
		delExprTree(root->content.caseOperator.right);
		break;
	case FUNC: 			// һ�����ӵ��ڲ��ڵ�
		delExprTree(root->content.caseFunc.child);
		break;
	default: 			// Ҷ�ӽڵ�
		break;
	}
	free(root);				// ɾ��Ҷ�ӽڵ�
}