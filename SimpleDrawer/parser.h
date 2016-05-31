#pragma once

extern double parameter,	// ����T�Ĵ洢�ռ�
originX, originY,			// �ᡢ��ƽ�ƾ���
scaleX, scaleY,				// �ᡢ�ݱ�������
rotAngle;					//��ת�Ƕ�

//-----------------------�﷨�������ض���
typedef double(*FuncPtr)(double);
typedef struct ExprNode {//�﷨���ڵ�
	enum TokenType opCode;
	union {
		struct {
			ExprNode *left;
			ExprNode *right;
		} caseOperator;//��Ԫ����
		struct {
			ExprNode *child;
			FuncPtr mathFuncPtr;
		} caseFunc;//��������
		double caseConst;//����������ֵ��ֵ���ɸı�
		double *caseParamPtr;//����������ֵ��ֵ���Ըı�
	} content;
} ExprNode;

//-----------------------parser�Ĳ��Ժ���
void parser(const char* fileName);