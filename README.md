# OS Experiment 3

这是第三次OSH课程实验的代码库，分为以下三部分：

## Message Queue

在这一部分中，我使用Message Queue方法实现了猜数字游戏。

#### 编译方法：

在MessageQueue子文件夹中使用`make`命令，该目录下存有MakeFile。
编译后将有可执行文件： host、guest

使用  `make clean` 来清除生成的可执行文件。

#### 使用方法：

使用两个终端，都保持工作目录在MessageQueues子文件夹下，先运行host，
再运行guest，host会产生一个1-10的随机数供猜测，
在guest中输入猜测即可进行猜测，
每一次猜测会获得反馈，如：`"Too Big."`、`"Too Small."`。
猜测正确后，host与guest都会在输出提示后结束。

#### Examples

```
$./host 
I have chosen a random number between 1 and 10, (both included)

I can tell you here that it is 2.

Waiting for a guess.
(Reading a message ...)
[GUEST]: guess = 8
Response is: Too Big.
(Sending the message ...)

Waiting for a guess.
(Reading a message ...)
[GUEST]: guess = 1
Response is: Too Small.
(Sending the message ...)

Waiting for a guess.
(Reading a message ...)
[GUEST]: guess = 2
Response is: You are right.
(Sending the message ...)

End Of Session.
```

同时:

```
./guest 
The host have chosen a random number between 1 and 10, (both included)

Input number to guess it:	88
The range of the number is from 1 to 10, (both included)

Input number to guess it:	8
(Sending the message ...)
(Reading a message ...)
[HOST]: Too Big.

Input number to guess it:	1
(Sending the message ...)
(Reading a message ...)
[HOST]: Too Small.

Input number to guess it:	2
(Sending the message ...)
(Reading a message ...)
[HOST]: You are right.

End Of Session.
```

## Semaphores

在这一部分中，我使用Semaphores方法实现了一个程序使得：创建两个进程，打开两个文件，
将两个文件中的数字依次交叉合并，输出至标准输出。

#### 编译方法：

在Semaphores子文件夹中使用`make`命令，该目录下存有MakeFile。
编译后将有可执行文件： merge

使用  `make test` 来快速进行编译和测试，编译后会自动调用`./merge odd.txt even.txt`，其中的文件也在同目录中。

使用  `make clean` 来清除生成的可执行文件。

#### 使用方法：

Usage:
```
merge [input_file_1] [input_file_2]
```
要求两个输入文件包含相同数量的数字，用空白分隔符(space, tag, new line)间隔。

#### Example:

按以下方式调用

```
./merge A.txt B.txt
```

若 A.txt 中有 `1 3 5 7 9`，而 B.txt 中有 `2 4 6 8 10`。

则会输出 

```
1 2 3 4 5 6 7 8 9 10
```
