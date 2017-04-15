# OS Experiment 3

这是第三次OSH课程实验的代码库，分为以下三部分：

## Message Queue

在这一部分中，我使用Message Queue方法实现了猜数字游戏。

#### 编译方法：

在MessageQueue子文件夹中使用`make`命令，该目录下存有MakeFile。
编译后将有可执行文件： host、guest

#### 使用方法：

使用两个终端，都保持工作目录在MessageQueue子文件夹下，先运行host，
再运行guest，host会产生一个1-10的随机数供猜测，
在guest中输入猜测即可进行猜测，
每一次猜测会获得反馈，如："Too Big."、"Too Small."。
猜测正确后，host与guest都会在输出提示后结束。

