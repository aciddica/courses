Diaries.txt 存储日记数据并作为输出文件，为四个程序共用

pdadd 	从pdadd_test.txt读取将要加入日记的实体
	
pdlist 	从pdlist_test.txt读取数据
	第一个数字表示有无日期范围 
	0表示无日期限制 即按日期升序展示所有日记
	1表示有日期限制 接下来输入开始日期 结束日期

pdremove	从pdremove_test.txt读取将要删除的日期

pdshow	从pdshow_test读取将要显示的日记日期
需要统一日期格式，在测试数据中日期格式都为"xxxx.yy.zz"