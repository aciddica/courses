源代码放在Code文件夹中。
源工程使用vs2019进行编译，由于继承问题，如果重新建立工程编译的话会引起LNK2005错误，需要在属性-连接器-命令行中添加：
/FORCE:MULTIPLE
指令。
测试execfile指令需要该文件与源代码在同一目录下，工程中已添加好，名称为“testfile”，使用指令时输入 
execfile testfile.txt；
即可。