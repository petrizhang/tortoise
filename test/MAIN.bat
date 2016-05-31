@echo off 
set/p fileName=请输入文件名并按回车（默认文件保存在input文件夹下）：
.\SimpleDrawer.exe %fileName%
echo 请查看output文件夹下errors.txt、SyntaxTree.txt、tokens.txt的输出
pause