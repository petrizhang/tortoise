#pragma once

extern double parameter,	// 参数T的存储空间
originX, originY,			// 横、纵平移距离
scaleX, scaleY,				// 横、纵比例因子
rotAngle;					//旋转角度

//-----------------------语法树结点相关定义
typedef double(*FuncPtr)(double);
typedef struct ExprNode {//语法树节点
	enum TokenType opCode;
	union {
		struct {
			ExprNode *left;
			ExprNode *right;
		} caseOperator;//二元运算
		struct {
			ExprNode *child;
			FuncPtr mathFuncPtr;
		} caseFunc;//函数调用
		double caseConst;//常量，绑定左值，值不可改变
		double *caseParamPtr;//变量，绑定右值，值可以改变
	} content;
} ExprNode;

//-----------------------parser的测试函数
void parser(const char* fileName);