# SimpleDrawer
Interpreter of a simple drawing language.

编译原理大作业，简单的函数绘图语言解释器。

## 项目构建
使用VS2015 community构建。

## 函数绘图语言语法说明

### 基本概念
注意：
<font color="red">本语言不区分大小写</font>

#### 数字字面量
本语言仅支持整数、小数形式的输入，均视为double类型，如下两种输入是合法的：
```
num
```
```
num.num
```
其中num是一个数字。
例:
```
1       -- 合法输入
0.1     -- 合法输入
.1      -- 非法输入
```

#### 内置常量
语言内置了两个常量pi和自然对数e，用户可直接使用。
其中pi=3.1415926，e=2.71828。

#### 变量
本语言不支持用户自定义变量，但内置了一个变量'T'用来支持循环绘图语句。
例如：
```
for T from 0 to 2*pi step pi/50 draw (t, t);
```
for语句的具体语法可参考后文。

#### 表达式
本语言支持数字的加、减、乘、除、乘方5种运算以及几个内置数学函数，具体说明如下：

|运算符|含义|示例|示例说明|  
|---|---|---|---|  
|+|加法运算符|1+1|略|  
|-|略|略|略|  
|*|略|略|略|  
|/|略|略|略|  
|**|乘方运算符|2**4|2的4次方|

|函数名|含义|  
|---|---|  
|sqrt(arg)|开平方|  
|sin(arg)|正弦函数|  
|cos(arg)|余弦函数|  
|tan(arg)|正切函数|  
|ln(arg1,arg2)|以arg1为底arg2的对数|  
|exp(arg)|10的arg次方|  

由四则运算、函数调用以及'('和')'可任意组合成数学表达式。


本绘图语言是一门非常简单的语言，不区分大小写，支持图像的平移、缩放、旋转等操作。

支持的语句如下，语句间需要有分号分隔：
### 平移
通过设置图像的原点即可进行平移，设置原点的语法为：
```
origin is (Expression, Exression)
```
单位为像素。

例：
```
origin is (20, 200);     
```
```
origin is (cos(20), sin(200));
```
### 缩放
由于本语言的绘图以像素为单位，欲使图像正常显示，需要放大相应的倍数。

缩放语法为：
```
scale is (Expression, Expression);
```
例：
```
scale is (40, 40);										-- 横纵轴各放大40倍
```
### 旋转
```
rot is Expression;
```
例：
```
rot is 0;											      -- 不旋转
```
```
rot is pi/2;												-- 顺时针旋转90度
```
```
rot is pi;												  -- 逆时针旋转180度
```

for T from 0 to 2*pi step pi/50 draw (t, -sin(t));		-- 画T的轨迹

for T from 0 to 2*pi step pi/50 draw (t, -sin(t));		-- 画T的轨迹

for T from 0 to 2*pi step pi/50 draw (t, -sin(t));		-- 画T的轨迹

### 关键字

|token名|关键字|字面值|函数指针|  
|---|---|---|---|  
| CONST_ID| "PI"	|3.1415926|NULL|   
| CONST_ID| "E"		|2.71828  |NULL|   
| T	  | "T"		|0.0	  |NULL|   
| FUNC	  | "SIN"	|0.0	  |sin |   
| FUNC	  | "COS"	|0.0	  |cos |   
| FUNC	  | "TAN"	|0.0	  |tan |   
| FUNC	  | "LN"	|0.0	  |log |   
| FUNC	  | "EXP"	|0.0	  |exp |   
| FUNC	  | "SQRT"	|0.0	  |sqrt|   
| ORIGIN  | "ORIGIN"	|0.0	  |NULL|   
| SCALE	  | "SCALE"	|0.0	  |NULL|   
| ROT	  | "ROT"	|0.0	  |NULL|   
| IS	  | "IS"	|0.0	  |NULL|   
| FOR	  | "FOR"	|0.0	  |NULL|   
| FROM	  | "FROM"	|0.0	  |NULL|   
| TO	  | "TO"	|0.0	  |NULL|   
| STEP	  | "STEP"	|0.0	  |NULL|   
| DRAW	  | "DRAW"	|0.0	  |NULL|   
