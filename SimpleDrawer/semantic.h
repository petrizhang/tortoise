#pragma once

//-----------------------��ñ��ʽ��ֵ
double getExprValue(struct ExprNode * root);
//-----------------------ɾ��һ���﷨��
void delExprTree(struct ExprNode * root);
//-----------------------ѭ�����Ƶ�����
void drawLoop(double start, double end, double step, struct ExprNode * horPtr, struct ExprNode * verPtr);