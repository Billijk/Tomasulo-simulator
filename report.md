# Tomasulo调度算法模拟实验文档

===

## 实验背景

在处理器中，先后执行的指令之间经常具有相关性（例如后一条指令用到前一条指令向寄存器写入的结果），因此早期简单的处理器使后续指令停顿，直到其所需的资源已经由前序指令准备就绪。Tomasulo算法则通过动态调度的方式，在不影响结果正确性的前提下，重新排列指令实际执行的顺序（乱序执行），提高时间利用效率。IBM System/360 Model 91处理器的浮点运算器中率先使用了这种算法。

该算法与之前同样用于实现指令流水线动态调度的计分板不同在于它使用了寄存器重命名机制。指令之间具有数据相关性（例如后条指令的源寄存器恰好是前条指令要写入的目标寄存器），进行动态调度时必须避免三类冒险：写后读（Read-after-Write, RAW）、写后写（Write-after-Write, WAW）、读后写（Write-after-Read, WAR）。第一种冒险也被称为真数据相关（true data dependence），而后两种冒险则并没有那么致命，它们可以由寄存器重命名来予以解决。Tomasulo算法使用了一个共享数据总线（common data bus, CDB）将已计算出的值广播给所有需要这个值作为指令源操作数的保留站。该算法尽可能降低了使用计分板技术导致的流水线停顿，从而改善了并行计算的效率。

## 实验原理

Tomasulo算法中，在指令的发射（issue）阶段，如果操作数和保留站都准备就绪，那么指令就可以直接发射并执行。如果操作数未就绪，则进入保留站的指令会跟踪即将产生这个所需操作数的那个功能单元。如果连可用的保留站功能单元都已经不够用，那么该指令必须被停顿。

为了化解读后写（WAR）和写后写（WAW）冲突，需要在该阶段进行指令的寄存器重命名。从指令队列中取出下一条指令，如果其所用到的操作数目前位于寄存器中，那么如果与指令匹配的功能单元（这类处理器通常具有多个功能单元以发挥指令级并行的优势）当前可用，则发射该指令；否则，由于没有可用的功能单元，指令被停顿，直到保留站或缓存可用。尽管执行时可能并未按照指令代码的先后顺序，但是它们在发射过程还是按照原先的顺序。这是为了确保指令顺序执行时的一些现象，例如处理器异常，能够以顺序执行时的同样顺序出现。下一个阶段为执行阶段。在该阶段，指令对应的操作被执行。执行前需要保证所有操作数可用，同时写后读（RAW）冲突已经被化解。系统通过计算有效地址来避免存储区的冲突，从而保证程序的正确性。最后的阶段为写结果阶段，算术逻辑单元（ALU）的计算结果被写回到寄存器，以及任何正在等待该结果的保留站中，如果是存储（store）指令，则写回到存储器中。

## 实验要求

-	设计实现 Tomasulo 算法模拟器，不限语言
	+	Tomasulo 算法模拟器能够执行浮点加、减、乘、除运算及 LOAD 和 STORE 操作，为了简化起见，我们在下表中给出了各种操作的具体描述。
	+	支持单步执行及连续执行（n 条指令），实时显示算法的运行状况，包括各条指令的运行状态、各寄存器以及内存的值、保留站（Reservation Stations）状态、Load Buffer和 Store Buffer 缓存的值等；
	+	程序执行完毕后，能够显示指令执行周期数等信息；
	+	为了简化设计，建议模拟器提供编辑内存值功能，以便实现数据输入；浮点除法可不做除 0 判断；
	+	能够以文本方式输入指令序列；
	+	显示界面自由设计。
	
|     指令格式    |                    指令说明                    | 指令周期 | 保留站/缓冲队列项数 |
|:---------------:|:----------------------------------------------:|:--------:|:-------------------:|
| ADDD F1, F2, F3 | F1, F2, F3为浮点寄存器，寄存器至少支持(F0~F10) |  2个周期 |   3（和SUBD共用）   |
| SUBD F1, F2, F3 |                      同上                      |  2个周期 |   3（和ADDD共用）   |
| MULD F1, F2, F3 |                      同上                      | 10个周期 |   2（和DIVD共用）   |
| DIVD F1, F2, F3 |                      同上                      | 40个周期 |   2（和MULD共用）   |
|   LD F1, ADDR   |      F1为寄存器，ADDR为地址，0<=ADDR<4096      |  2个周期 |          3          |
|   ST F1, ADDR   |                      同上                      |  2个周期 |          3          |

## 实验小组成员

| 姓名| 学号 | 分工 |
|:-----:|:-----:|:----:|
| 王子云 | 2014010303 | 算法内核 |
| 黄河 | 2013012408 | UI界面 |
| 莫宇尘 | 2014012283 | 实验报告 | 

## 实验结果

我们通过C++实现了算法内核，并使用Qt为其编写了图形界面.

### 算法内核实现

算法内核对应源代码中simulator/目录下的文件，架构如下。

-	simulator: 是内核的最高层，实现了模拟时钟周期的功能，并在kernel的基础上封装了和Qt进行数据交互的接口。
-	kernel: 是内核的主要部分，维护了指令队列和保留站
-	reservation: 实现了保留站，并派生出适用于ALU和Memory的保留站ALUReservation和缓存MemoryBuffer。
-	cdb: 实现共享数据总线，使其具有给保留站播送计算结果的功能。
-	alu: 实现了基类ALU，并在其基础上封装了子类加（减）法器Adder和乘（除）法器Multiplier。其中，除法器没有流水线。
-	memory: 实现内存和访存机制
-	register: 实现浮点寄存器，维护值和保留情况，同时实现了RegisterManager类对所有寄存器进行维护。
-	instruction: 实现指令基类，并在此基础上封装了ArithmeticInstruction、MemoryInstruction、ErrorInstruction类。在ArithmeticInstruction上派生ADDDi、SUBDi、 MULDi、DIVDi类，在MemoryInstruction上派生LDi和STi类。

当Simulator模拟一个时钟周期时，首先调用Kernel的step函数，该函数依次调用加法器、乘法器、内存的**do_cycle**函数，使其执行一个时钟周期，再**发射指令**，最后令总线将本周期计算的**结果发给各保留站**。

在各运算器和内存的do_cycle函数中

-	首先将上一轮的计算结果发给总线。
-	再判断该部件第一段流水对应的保留站是否可用，若可用，则在当前已经可以进行计算的保留站中随机选择一个，将它和第一段流水标记为一一对应。
-	再查看最后一段流水是否有对应的保留站，若有，则计算其结果。
-	最后将每个保留站对应的流水号推后一个。

在我们的实现中，采用了先从总线上取上一轮数据代替预约的保留站名，再进行本轮操作的实现策略，这样做优势在于不需要为总线的实现维护额外的存储结构。

发射指令时Kernel根据指令类型，先查询对应的运算器或内存的保留站/缓冲区是否可用，若可用则发射到对应的运算器/内存中。运算器/内存收到发射的指令后，根据指令中寄存器的状态将指令与对应的操作数或预定的保留站名字等写入一个可用的保留站。

在每个时钟周期的最后，总线遍历本周期中各个部件发来的结果，对每个结果，遍历Kernel中所有的保留站，若有某个保留站预约的值在本轮中被计算出来，则用新的值更新保留站状态。


### 图形界面实现

通过Qt模块与内核的通信，我们实现了

+	浮点加、减、乘、除运算，LOAD & STORE操作
+	单步执行、n步执行、全部执行，期间实时显示指令状态、保留站状态、寄存器和内存的值、Load buffer和Store buffer的值
+	实时显示执行周期数
+	编辑内存和寄存器的值
+	通过程序内添加或通过文件方式输入指令序列，并对已有的指令进行编辑或删除

相关功能截图如下：

<center>
</br></br>
</br>
<h3>主界面</h3>
<img src="SNAPSHOTS/main.PNG" width=475/>
</br></br>
<h3>从文件读入指令</h3>
<img src="SNAPSHOTS/FROMFILE.PNG" width=475/>
</br></br>
<h3>单步执行1</h3>
<img src="SNAPSHOTS/SINGLE_STEP.PNG" width=475/>
</br></br>
<h3>单步执行2</h3>
<img src="SNAPSHOTS/SINGLE_STEP2.PNG" width=475/>
</br></br>
<h3>单步执行3</h3>
<img src="SNAPSHOTS/SINGLE_STEP3.PNG" width=475/>
</br></br>
<h3>N步执行1</h3>
<img src="SNAPSHOTS/N_STEPS.PNG" width=475/>
</br></br>
<h3>N步执行2</h3>
<img src="SNAPSHOTS/N_STEPS2.PNG" width=475/>
</br></br>
<h3>全部运行</h3>
<img src="SNAPSHOTS/RUN.PNG" width=475/>
</br></br>
<h3>设置内存</h3>
<img src="SNAPSHOTS/SETMEM.PNG" width=475/>
</br></br>
<h3>设置寄存器</h3>
<img src="SNAPSHOTS/SETREG.PNG" width=475/>
</br></br>
<h3>编辑指令</h3>
<img src="SNAPSHOTS/SET_INSTRUCTION.PNG" width=475/>
</br></br>
<h3>删除指令</h3>
<img src="SNAPSHOTS/REMOVE_INSTRUCTION.PNG" width=460/>
</center>