#pragma once

//-----------------------获得表达式的值
double getExprValue(struct ExprNode * root);
//-----------------------删除一课语法树
void delExprTree(struct ExprNode * root);
//-----------------------循环绘制点坐标
void drawLoop(double start, double end, double step, struct ExprNode * horPtr, struct ExprNode * verPtr);