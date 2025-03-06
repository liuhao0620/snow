# snow
a new script programming language named snow
简单易用，只有一个源文件

# 编译可执行文件
[windows]
可以使用vs进行编译
只需要编译snow.cpp即可
vs2019测试通过
需要支持c++17
如有c++17相关警告，可添加
_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
_CRT_SECURE_NO_WARNINGS
两个预编译宏
[linux]
可使用gcc进行编译
需使用支持c++17的版本
g++ -std=c++17 -o snow -I ./include/ .source/snow.cpp

# 测试
[命令行模式]
直接打开编译后的可执行文件即可
[从文件启动]
snow.exe example/unit_test.sno
./snow ./example/unit_test.sno
