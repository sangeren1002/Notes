
# 一、 单任务系统（裸机）

主要是采用超级循环系统（前后台系统），应用程序是一个无限的循环，循环中调用相应的函数完成相应的操作，这部分可以看做后台行为；中断服务程序处理异步事件，这部分可以看做是前台行为。后台也可以叫做任务级，前台也叫作中断级。

前后台系统的编程思路有两种：轮询方式（实时性得不到保障，紧急与非紧急消息不能有效管理）、中断方式（可以保证一定的实时性，紧急消息可以得到响应）。
![裸机开发查询方式流程图](https://img-blog.csdnimg.cn/20181226145523355.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

![裸机开发中断方式流程图](https://img-blog.csdnimg.cn/20181226145623313.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

采用中断和查询结合的方式可以解决大部分裸机应用，但随着工程的复杂，裸机方式的缺点就暴露出来了：

1. 必须在中断(ISR)内处理时间关键运算：
- ISR 函数变得非常复杂，并且需要很长执行时间。
- ISR 嵌套可能产生不可预测的执行时间和堆栈需求。

2.  超级循环和 ISR 之间的数据交换是通过全局共享变量进行的：
- 应用程序的程序员必须确保数据一致性。

3. 超级循环可以与系统计时器轻松同步，但：
- 如果系统需要多种不同的周期时间，则会很难实现。
- 超过超级循环周期的耗时函数需要做拆分。
- 增加软件开销，应用程序难以理解。

4. 超级循环使得应用程序变得非常复杂，因此难以扩展：
- 一个简单的更改就可能产生不可预测的副作用，对这种副作用进行分析非常耗时。
- 超级循环概念的这些缺点可以通过使用实时操作系统 (RTOS) 来解决。


# 二、多任务系统（带OS）
采用多任务系统可以以上的裸机开发遇到的4大缺点。

![多任务系统流程图](https://img-blog.csdnimg.cn/20181226145721490.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

RTOS的实现重点就在这个OS任务调度器上，调度器的作用就是使用相关的调度算法来决定当前需要执行的任务。FreeRTOS就是一款支持多任务运行的实时操作系统，具有时间片、抢占式和合作式三种调度方式。通过 FreeRTOS 实时操作系统可以将程序函数分成独立的任务，并为其提供合理的调度方式。

## 1. 任务堆栈
![cubemx配置F407的堆栈配置文件](https://img-blog.csdnimg.cn/20181226151221136.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

栈大小 0x400 = 1024,单位字节
在RTOS下，上面截图里设置的栈大小有了一个新名字叫做系统栈空间，而任务栈是不使用这里的空间,哪里使用这里的栈空间呢，实际上是中断函数和中断嵌套。

> - 由于 Cortex-M3 和 M4 内核具有双堆栈指针，MSP 主堆栈指针和 PSP 进程堆栈指针，或者叫 PSP
任务堆栈指针也是可以的。在 FreeRTOS 操作系统中，主堆栈指针 MSP 是给系统栈空间使用的，进
程堆栈指针 PSP 是给任务栈使用的。也就是说，在 FreeRTOS 任务中，所有栈空间的使用都是通过
PSP 指针进行指向的。一旦进入了中断函数以及可能发生的中断嵌套都是用的 MSP 指针。这个知识
点要记住它，当前可以不知道这是为什么，但是一定要记住。
>- 实际应用中系统栈空间分配多大，主要是看可能发生的中断嵌套层数，下面我们就按照最坏执行情况
进行考虑，所有的寄存器都需要入栈，此时分为两种情况：
&ensp; &ensp; * 64 字节：
对于 Cortex-M3 内核和未使用 FPU（浮点运算单元）功能的 Cortex-M4 内核在发生中断时需要将 16 个通用寄存器全部入栈，每个寄存器占用 4 个字节，也就是 16*4 = 64 字节的空间。可能发生几次中断嵌套就是要 64 乘以几即可。当然，这种是最坏执行情况，也就是所有的寄存器都入栈。（注：任务执行的过程中发生中断的话，有 8 个寄存器是自动入栈的，这个栈是任务栈，进入中断以后其余寄存器入栈以及发生中断嵌套都是用的系统栈）
&ensp; &ensp; * 200 字节
对于具有 FPU（浮点运算单元）功能的 Cortex-M4 内核，如果在任务中进行了浮点运算，那么在发生中断的时候除了 16 个通用寄存器需要入栈，还有 34 个浮点寄存器也是要入栈的，也就是(16+34)*4 = 200 字节的空间。当然，这种是最坏执行情况，也就是所有的寄存器都入栈。


### 1.1 任务栈大小确定
 - 函数的栈大小计算起来是比较麻烦的，那么有没有简单的办法来计算呢？有的，一般 IDE 开发环境都有这样的功能，比如 MDK 会生成一个 htm 文件，通过这个文件用户可以知道每个被调用函数的最大栈需求以及各个函数之间的调用关系。但是 MDK 无法确定通过函数指针实现函数调用时的栈需求。另外，发生中断或中断嵌套时的现场保护需要的栈空间也不会统计。
-  一般来说，用户可以事先给任务分配一个大的栈空间，然后通过打印任务栈的使用情况，运行一段时间就会有个大概的范围了。这种方法比较简单且实用些。

###  1.2 栈溢出检测机制
栈生长方向从高地址向低地址生长（M4 和 M3 是这种方式）

![栈溢出示例](https://img-blog.csdnimg.cn/20181226160802296.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

> - 上图标识 3 的位置是局部变量 int i 和 int array[10]占用的栈空间，但申请了栈空间后已经越界了。这个就是所谓的栈溢出了。如果用户在函数 test 中通过数组 array 修改了这部分越界区的数据且==这部分越界的栈空间暂时没有用到或者数据不是很重要，情况还不算严重，但是如果存储的是关键数据，会直接导致系统崩溃==。
>- 上图标识 4 的位置是局部变量申请了栈空间后，栈指针向下偏移（返回地址+变量 i+10 个数组元素）*4 =48 个字节。
> - 上图标识 5 的位置可能是其它任务的栈空间，也可能是全局变量或者其它用途的存储区，如果 test函数在使用中还有用到栈的地方就会从这里申请，==这部分越界的空间暂时没有用到或者数据不是很重要，情况还不算严重，但是如果存储的是关键数据，会直接导致系统崩溃。==

FreeRTOS 提供了两种栈溢出检测机制，这两种检测都是在任务切换时才会进行：
- 在任务切换时检测任务栈指针是否过界了，如果过界了，在任务切换的时候会触发栈溢出钩子函数。
void vApplicationStackOverflowHook( TaskHandle_t xTask,signed char *pcTaskName );
用户可以在钩子函数里面做一些处理。这种方法不能保证所有的栈溢出都能检测到。比如任务在执行的过程中出现过栈溢出。任务切换前栈指针又恢复到了正常水平，这种情况在任务切换的时候是检测不到的。又比如任务栈溢出后，把这部分栈区的数据修改了，这部分栈区的数据不重要或者暂时没有用到还好，但如果是重要数据被修改将直接导致系统进入硬件异常，这种情况下，栈溢出检测功能也是检测不到的。
使用方法一需要用户在 FreeRTOSConfig.h 文件中配置如下宏定义：
```
#define configCHECK_FOR_STACK_OVERFLOW 1
```
- 任务创建的时候将任务栈所有数据初始化为 0xa5，任务切换时进行任务栈检测的时候会检测末尾的 16 个字节是否都是 0xa5，通过这种方式来检测任务栈是否溢出了。相比方法一，这种方法的速度稍慢些，但是这样就有效地避免了方法一里面的部分情况。不过依然不能保证所有的栈溢出都能检测到，比如任务栈末尾的 16 个字节没有用到，即没有被修改，但是任务栈已经溢出了，这种情况是检测不到的。另外任务栈溢出后，任务栈末尾的 16 个字节没有修改，但是溢出部分的栈区数据被修改了，这部分栈区的数据不重要或者暂时没有用到还好，但如果是重要数据被修改将直接导致系统进入硬件异常，这种情况下，栈溢出检测功能也是检测不到的。
使用方法二需要用户在 FreeRTOSConfig.h 文件中配置如下宏定义：
~~~
#define configCHECK_FOR_STACK_OVERFLOW 2
~~~
除了 FreeRTOS 提供的这两种栈溢出检测机制，还有其它的栈溢出检测机制，大家可以在 Mircrium 官方发布的如下这个博文中学习：
https://www.micrium.com/detecting-stack-overflows-part-2-of-2/
>钩子函数：
钩子函数的主要作用就是对原有函数的功能进行扩展，用户可以根据自己的需要往里面添加相关的测试代码，大家可以在 FreeRTOS 工程中检索这个钩子函数 vApplicationStackOverflowHook 所在的位置。

## 2. 任务状态
FreeRTOS的任务状态（4种）
1.运行态(Running)  2.就绪态(Ready)  3.阻塞态(Blocked)  4.挂起态(Suspended)
ucos的任务状态（5种）
1.睡眠状态    2.就绪状态      3.等待状态  4.中断服务状态    5.执行状态
1. Running—运行态
当任务处于实际运行状态被称之为运行态，即 CPU 的使用权被这个任务占用。
2. Ready—就绪态
处于就绪态的任务是指那些能够运行（没有被阻塞和挂起），但是当前没有运行的任务，因为同优先
级或更高优先级的任务正在运行。
23. Blocked—阻塞态
由于等待信号量，消息队列，事件标志组等而处于的状态被称之为阻塞态，另外任务调用延迟函数也
会处于阻塞态。
 4. Suspended—挂起态
类似阻塞态，通过调用函数 vTaskSuspend()对指定任务进行挂起，挂起后这个任务将不被执行，只
有调用函数 xTaskResume()才可以将这个任务从挂起态恢复。

![FreeRTOS任务状态转换图](https://img-blog.csdnimg.cn/20181226153949491.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
## 3. 任务优先级
### 3.1任务优先级说明
- FreeRTOS 中任务的最高优先级是通过 FreeRTOSConfig.h 文件中的 configMAX_PRIORITIES 进行配置的，用户实际可以使用的优先级范围是 0 到 configMAX_PRIORITIES – 1。比如我们配置此宏定义为 5，那么用户可以使用的优先级号是 0,1,2,3,4，不包含 5，对于这一点，初学者要特别的注意。
- 用户配置任务的优先级数值越小，那么此任务的优先级越低，空闲任务的优先级是 0。
- 建议用户配置宏定义 configMAX_PRIORITIES 的最大值不要超过 32，即用户任务可以使用的优先级范围是0到31。
### 3.2 任务优先级分配方案
![任务优先级分配方案](https://img-blog.csdnimg.cn/20181226162912531.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

- __IRQ 任务__：IRQ 任务是指通过中断服务程序进行触发的任务，此类任务应该设置为所有任务里面优先级最高的。
- __高优先级后台任务__：比如按键检测，触摸检测，USB 消息处理，串口消息处理等，都可以归为这一类任务。
- __低优先级的时间片调度任务__：比如 emWin 的界面显示，LED 数码管的显示等不需要实时执行的都可以归为这一类任务。 实际应用中用户不必拘泥于将这些任务都设置为优先级 1 的同优先级任务，可以设置多个优先级，只需注意这类任务不需要高实时性。
- __空闲任务__：空闲任务是系统任务。
> 特别注意：IRQ 任务和高优先级任务必须设置为阻塞式（调用消息等待或者延迟等函数即可），只有这样，高优先级任务才会释放 CPU 的使用权，,从而低优先级任务才有机会得到执行。这里的优先级分配方案是我们推荐的一种方式，实际项目也可以不采用这种方法。 调试出适合项目需求的才是最好的。
### 3.3 任务优先级与终端优先级的区别
这两个之间没有任何关系，不管中断的优先级是多少，中断的优先级永远高于任何任务的优先级，即任务在执行的过程中，中断来了就开始执行中断服务程序。
另外对于 STM32F103，F407 和 F429 来说，==中断优先级的数值越小，优先级越高==。 而 ==FreeRTOS的任务优先级是，任务优先级数值越小，任务优先级越低==。
## 4. 任务调度
FreeRTOS就是一款支持多任务运行的实时操作系统，具有时间片、抢占式和合作式三种调度方式。
- 合作式调度，主要用在资源有限的设备上面，现在已经很少使用了。出于这个原因，后面的
FreeRTOS 版本中不会将合作式调度删除掉，但也不会再进行升级了。
- 抢占式调度，每个任务都有不同的优先级，任务会一直运行直到被高优先级任务抢占或者遇到阻塞式的 API 函数，比如 vTaskDelay。
- 时间片调度，每个任务都有相同的优先级，任务会运行固定的时间片个数或者遇到阻塞式的 API 函数，比如vTaskDelay，才会执行同优先级任务之间的任务切换。
### 4.1 调度器
调度器就是使用相关的调度算法来决定当前需要执行的任务。所有的调度器有一个共同的
特性：
- 调度器可以区分就绪态任务和挂起任务（由于延迟，信号量等待，邮箱等待，事件组等待等原因而使
得任务被挂起）。
- 调度器可以选择就绪态中的一个任务，然后激活它（通过执行这个任务）。 当前正在执行的任务是运
行态的任务。
- 不同调度器之间最大的区别就是如何分配就绪态任务间的完成时间。

嵌入式实时操作系统的核心就是调度器和任务切换，调度器的核心就是调度算法。任务切换的实现在不同的嵌入式实时操作系统中区别不大，基本相同的硬件内核架构，任务切换也是相似的。调度算法就有些区别了。
#### 4.1.1抢占式调度器
使用了抢占式调度，最高优先级的任务一旦就绪，总能得到 CPU 的控制权。 比如，当一个运行着的任务被其它高优先级的任务抢占，当前任务的 CPU 使用权就被剥夺了，或者说被挂起了，那个高优先级的任务立刻得到了 CPU 的控制权并运行。 又比如，如果中断服务程序使一个高优先级的任务进入就绪态，中断完成时，被中断的低优先级任务被挂起，优先级高的那个任务开始运行。使用抢占式调度器，使得最高优先级的任务什么时候可以得到 CPU 的控制权并运行是可知的，同时使得任务级响应时间得以最优化。

总的来说，学习抢占式调度要掌握的最关键一点是：==每个任务都被分配了不同的优先级，抢占式调度器会获得就绪列表中优先级最高的任务，并运行这个任务。==
在 FreeRTOS 的配置文件 FreeRTOSConfig.h 中禁止使用时间片调度，那么每个任务必须配
置不同的优先级。当 FreeRTOS 多任务启动执行后，基本会按照如下的方式去执行：
1. 首先执行的最高优先级的任务 Task1，Task1 会一直运行直到遇到系统阻塞式的 API 函数，比如延迟，事件标志等待，信号量等待，Task1 任务会被挂起，也就是释放 CPU 的执行权，让低优先级的任务得到执行。
2. FreeRTOS 操作系统继续执行任务就绪列表中下一个最高优先级的任务 Task2，Task2 执行过程中有两种情况：
> - Task1由于延迟时间到，接收到信号量消息等方面的原因，使得 Task1从挂起状态恢复到就绪态，
在抢占式调度器的作用下，Task2 的执行会被 Task1 抢占。
>- Task2 会一直运行直到遇到系统阻塞式的 API 函数，比如延迟，事件标志等待，信号量等待，Task2任务会被挂起，继而执行就绪列表中下一个最高优先级的任务。

3.  如果用户创建了多个任务并且采用抢占式调度器的话，基本都是按照上面两条来执行。 根据抢占式调度器，当前的任务要么被高优先级任务抢占，要么通过调用阻塞式 API 来释放 CPU 使用权让低优先级任务执行，没有用户任务执行时就执行空闲任务.
   ![抢占式调度器](https://img-blog.csdnimg.cn/2018122616455434.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

#### 4.1.2 时间片调度器
在小型的嵌入式 RTOS 中，最常用的的时间片调度算法就是 Round-robin 调度算法。这种调度算法可以用于抢占式或者合作式的多任务中。另外，时间片调度适合用于不要求任务实时响应的情况。
实现 Round-robin 调度算法需要给同优先级的任务分配一个专门的列表，用于记录当前就绪的任务，并为每个任务分配一个时间片（**也就是需要运行的时间长度，时间片用完了就进行任务切换**）。
在 FreeRTOS 操作系统中只有同优先级任务才会使用时间片调度，另外还需要用户在FreeRTOSConfig.h 文件中使能宏定义：
~~~
#define configUSE_TIME_SLICING 1
~~~
默认情况下，此宏定义已经在 FreeRTOS.h 文件里面使能了，用户可以不用在FreeRTOSConfig.h 文件中再单独使能。
**示例：**
- 创建 4 个同优先级任务 Task1，Task2，Task3 和 Task4。
- 先运行任务 Task1，运行够 5 个系统时钟节拍后，通过时间片调度切换到任务 Task2。
- 任务 Task2 运行够 5 个系统时钟节拍后，通过时间片调度切换到任务 Task3。
- 任务 Task3 在运行期间调用了阻塞式 API 函数，调用函数时，虽然 5 个系统时钟节拍的时间片大小还没有用完，此时依然会通过时间片调度切换到下一个任务 Task4。 （注意，没有用完的时间片不会再使用，下次任务 Task3 得到执行还是按照 5 个系统时钟节拍运行）
- 任务 Task4 运行够 5 个系统时钟节拍后，通过时间片调度切换到任务 Task1。
![时间片调度器](https://img-blog.csdnimg.cn/20181226164827999.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
## 5. 临界区、锁与系统时间
### 5.1 临界区与开关中断
代码的临界临界区，一旦这部分代码开始执行，则不允许任何中断打断。为确保临界区代码
的执行不被中断，在进入临界区之前须关中断，而临界区代码执行完毕后，要立即开中断。
>FreeRTOS 的源码中有多处临界段的地方， 临界段虽然保护了关键代码的执行不被打断， 但也会影响系统的实时性。比如此时某个任务正在调用系统 API 函数，而且此时中断正好关闭了，也就是进入到了临界区中，这个时候如果有一个紧急的中断事件被触发，这个中断就不能得到及时执行，必须等到中断开启才可以得到执行， 如果关中断时间超过了紧急中断能够容忍的限度， 危害是可想而知的。
>
FreeRTOS 源码中就有多处临界段的处理，跟 FreeRTOS 一样，uCOS-II 和 uCOS-III 源码中都是有临界段的，而 RTX 的源码中不存在临界段。 另外，除了 FreeRTOS 操作系统源码所带的临界段以外，用户写应用的时候也有临界段的问题，比如以下两种：
-  读取或者修改变量（特别是用于任务间通信的全局变量）的代码，一般来说这是最常见的临界代码。
- 调用公共函数的代码，特别是不可重入的函数，如果多个任务都访问这个函数，结果是可想而知的。
总之，对于临界段要做到执行时间越短越好，否则会影响系统的实时性。
>- 重入函数的实现特征
一般而言，重入函数具有如下特征：
>1. ==函数内部无整个软件生命周期的变量（静态变量）==
>2. ==未引用或者访问整个软件生命周期的变量（全局变量）==

可重入函数示例：
~~~
/*A parameter is passed into the function. This will either be passed on the stack or in a CPU register. Either way is safe as each task maintains its own stack and its own set of register values.*/
long lAddOneHunderad ( long lVal1)
{
    /*This function scope variable will also be allocated to the stack or a register， depending on compiler and optimization level. Each task or interrupt that calls this function will have its own copy of lVar2.*/
    long lVal2;
    lVar2 = lVar1 + 100;

    /*Most likely the return value will be placed in a CPU register,although it too could be placed on the stack.*/
    return lVar2;
}
~~~
不可重入函数示例
~~~
/*In this case lVar1 is a global variable so every task that calls the function will be accessing the same single copy of the variable.*/
long lVal1;
long lNonsenseFunction ( void )
{
    /*This variable is static so is not allocated on the stack.Each task that calls the function will be accessing the single copy of the variable.*/
    static long lState = 0; // 与函数内部无整个软件生命周期的变量（静态变量）相悖
    long lReturn;

    switch( lState )
    {
        case 0:
            lReturn = lVar1 + 10;
            lState = 1;
            break;
        case 1:
            lReturn = lVar1 +20;
            1State = 0;
            break;
    }
}
~~~
- 任务代码临界区处理
FreeRTOS 任务代码中临界段的进入和退出主要是通过操作寄存器 basepri 实现的。进入临界段前操作寄存器 basepri 关闭了所有小于等于宏定义configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY所定义的中断优先级，这样临界段代码就不会被中断干扰到，而且实现任务切换功能的 PendSV 中断和滴答定时器中断是最低优先级中断，所以此任务在执行临界段代码期间是不会被其它高优先级任务打断的。退出临界段时重新操作 basepri 寄存器，即打开被关闭的中断（这里我们不考虑不受 FreeRTOS 管理的更高优先级中断）。
 FreeRTOS 进入和退出临界段的函数如下：
~~~
#define taskENTER_CRITICAL() portENTER_CRITICAL()
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()
~~~
再进两步跟踪宏定义的实现如下：
~~~
void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	/* This is not the interrupt safe version of the enter critical function so assert() if it is
	being called from an interrupt context. Only API functions that end in "FromISR" can be used
	in an interrupt. Only assert if the critical nesting count is 1 to protect against
	recursive calls if the assert function also uses a critical section. */
	if( uxCriticalNesting == 1 )
	{
		configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
	}
}
/*-----------------------------------------------------------*/
void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
~~~
上面的这两个函数都对变量 uxCriticalNesting 进行了操作。这个变量比较重要，用于临界段的嵌套计数。初学的同学也许会问这里直接的开关中断不就可以了吗，为什么还要做一个嵌套计数呢？主要是因为直接的开关中断方式不支持在开关中断之间的代码里再次执行开关中断的嵌套处理，假如当前我们的代码是关闭中断的，嵌套了一个含有开关中断的临界区代码后，退出时中断就成开的了，这样就出问题了。 通过嵌套计数就有效地防止了用户嵌套调用函数 taskENTER_CRITICAL 和 taskEXIT_CRITICAL 时出错。
~~~
直接的开关中断方式不支持嵌套调用例子说明
比如下面的例子：
void FunctionA()
{
	taskDISABLE_INTERRUPTS(); 关闭中断
	FunctionB(); 调用函数 B
	FunctionC(); 调用函数 C
	taskENABLE_INTERRUPTS(); 打开中断
}
void FunctionB()
{
	taskDISABLE_INTERRUPTS(); 关闭中断
	//代码
	taskENABLE_INTERRUPTS(); 打开中断
}
工程中调用了 FunctionA 就会出现执行完 FunctionB 后中断被打开的情况，此时 FunctionC 将不被保护了。
~~~

- 中断服务程序的临界区处理
与任务代码里临界段的处理方式类似，中断服务程序里面临界段的处理也有一对开关中断函数。
~~~
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
~~~
再进两步跟踪宏定义的实现如下：
~~~
static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
	__asm
	{
	/* Barrier instructions are not used as this function is only used to lower the BASEPRI value. */
	msr basepri, ulBASEPRI
	}
}
/*-----------------------------------------------------------*/
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical section. */
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
	return ulReturn;
}
中断服务程序里面的临界段代码的开关中断也是通过寄存器 basepri 实现的。这里怎么没有中断嵌套计数了呢？
是的，这里换了另外一种实现方法，通过保存和恢复寄存器 basepri 的数值就可以实现嵌套使用。如果大家研究
过 uCOS-II 或者 III 的源码，跟这里的实现方式是一样的，使用的时候一定要保证成对使用。
~~~
### 5.2 锁
1. 调度锁
调度锁就是 RTOS 提供的调度器开关函数，如果某个任务调用了调度锁开关函数，处于调度锁开和调度锁关之间的代码在执行期间是不会被高优先级的任务抢占的，即任务调度被禁止。这一点要跟临界段的作用区分开，调度锁只是禁止了任务调度，并没有关闭任何中断，中断还是正常执行的。而临界段进行了开关中断操作。
调度锁相关函数;
~~~
void vTaskSuspendAll( void );
BaseType_t xTaskResumeAll(void)
~~~
函数 ==vTaskSuspendAll== 用于实现 FreeRTOS 调度锁关闭。
使用这个函数要注意以下问题：
- 调度锁函数只是禁止了任务调度，并没有关闭任何中断。
- 调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 一定要成对使用
- 切不可在调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 之间调用任何会引起任务切换的 API，比如 vTaskDelayUntil、 vTaskDelay、 xQueueSend 等。

函数 ==xTaskResumeAll==用于实现 FreeRTOS 调度锁开启
调度锁关闭后，如果需要任务切换，此函数返回 pdTRUE，否则返回 pdFALSE。
使用这个函数要注意以下问题：
-  调度锁函数只是禁止了任务调度，并没有关闭任何中断。
- 调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 一定要成对使用。
- 切不可在调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 之间调用任何会引起任务切换的 API，比如 vTaskDelayUntil、 vTaskDelay、 xQueueSend 等。
2. 任务锁
简单的说，为了防止当前任务的执行被其它高优先级的任务打断而提供的锁机制就是任务锁。
FreeRTOS 也没有专门的任务锁函数，但是使用 FreeRTOS 现有的功能有两种实现方法：
(1) 通过给调度器加锁实现
利用 FreeRTOS 的调度锁功能给调度器加锁的话，将关闭任务切换功能，从而高优先级任务也就无法抢占低优先级任务的执行，同时高优先级任务也是无法向低优先级任务切换的。 另外特别注意，调度锁只是禁止了调度器工作，并没有关闭任何中断。
(2) 通过关闭任务切换中断 PendSV 和系统时钟节拍中断 Systick利用 FreeRTOS 的任务代码临界段处理函数就可以关闭 PendSV 中断和 Systick 中断。因为进入临界段前，操作寄存器 basepri 关闭了所有小于等于宏定义configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 所定义的中断优先级（实现任务切换功能的 PendSV 中断和滴答定时器中断是最低优先级中断，所以也是被关闭的），这样低优先级任务在执行临界段代码期间是不会被高优先级任务打断的，从而就实现了任务锁的效果。
3. 中断锁
中断锁就是 RTOS 提供的开关中断函数，FreeRTOS 没有专门的中断锁函数，使用中断服务程序临界段处理函数就可以实现同样效果。
### 5.1 FreeRTOS 系统时钟节拍和时间管理
#### 5.1.1 FreeRTOS 时钟节拍
 任何操作系统都需要提供一个时钟节拍，以供系统处理诸如延时、 超时等与时间相关的事件。时钟节拍是特定的周期性中断，这个中断可以看做是系统心跳。 中断之间的时间间隔取决于不同的应用，一般是 1ms – 100ms。时钟的节拍中断使得内核可以将任务延迟若干个时钟节拍，以及当任务等待事件发生时，提供等待超时等依据。时钟节拍率越快，系统的额外开销就越大。任何操作系统都需要提供一个时钟节拍，以供系统处理诸如延时、 超时等与时间相关的事件。时钟节拍是特定的周期性中断，这个中断可以看做是系统心跳。 中断之间的时间间隔取决于不同的应用，一般是 1ms – 100ms。时钟的节拍中断使得内核可以将任务延迟若干个时钟节拍，以及当任务等待事件发生时，提供等待超时等依据。时钟节拍率越快，系统的额外开销就越大。
对于 Cortex-M3 内核的 STM32F103 和 Cortex-M4 内核的 STM32F407 以及 F429，教程配套的例子都是用滴答定时器来实现系统时钟节拍的。
- 滴答定时器 Systick
SysTick 定时器被捆绑在 NVIC 中，用于产生 SysTick 异常（异常号： 15）， 滴答定时器是一个 24 位的递减计数器，支持中断。 使用比较简单， 专门用于给操作系统提供时钟节拍。

FreeRTOS 的系统时钟节拍可以在配置文件 FreeRTOSConfig.h 里面设置：
~~~
#define configTICK_RATE_HZ ( ( TickType_t ) 1000 )
~~~
如上所示的宏定义配置表示系统时钟节拍是 1KHz，即 1ms。
#### 5.1.2 FreeRTOS 时间管理

时间管理功能是 FreeRTOS 操作系统里面最基本的功能，同时也是必须要掌握好的。

1.  时间延时
FreeRTOS 中的时间延迟函数主要有以下两个作用：
>- 为周期性执行的任务提供延迟。
>- 对于抢占式调度器，让高优先级任务可以通过时间延迟函数释放 CPU 使用权，从而让低优先级任务可以得到执行。

通过如下的框图来说明一下延迟函数对任务运行状态的影响，有一个形象的认识。
![延迟函数对任务运行状态的影响](https://img-blog.csdnimg.cn/20181227110812293.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

对任务 Task1 的运行状态做说明，调度器支持时间片调度和抢占式调度。
运行过程描述如下：
- 起初任务 Task1 处于运行态，调用 vTaskDelay 函数后进入到阻塞状态，也就是 blocked 状态。
- vTaskDelay 函数设置的延迟时间到，由于任务 Task1 不是当前就绪的最高优先级任务，所以不能进
入到运行状态，只能进入到就绪状态，也就是 ready 状态。
- 一段时间后，调度器发现任务 Task1 是当前就绪的最高优先级任务，从而任务从就绪态切换到运行态。
- 由于时间片调度，任务 Task1 由运行态切换到就绪态。
FreeRTOS 时间相关的函数主要有以下 4 个：

~~~
void vTaskDelay(const TickType_t xTicksToDelay ); /* 延迟时间长度 ,用于任务的延迟。*/

void vTaskDelayUntil( TickType_t *pxPreviousWakeTime, /* 存储任务上次处于非阻塞状态时刻的变量地址 */
const TickType_t xTimeIncrement ); /* 周期性延迟时间 */				
volatile TickType_t xTaskGetTickCount( void );/* 用于获取系统当前运行的时钟节拍数。此函数用于在任务代码里面调用，如果在中断服务程序里面调用的话，需要使用函数xTaskGetTickCountFromISR，这两个函数切不可混用。 */
volatile TickType_t xTaskGetTickCountFromISR( void );/* 用于获取系统当前运行的时钟节拍数。此函数用于在中断服务程序里面调用，如果在任务里面调用的话，需要使用函数 xTaskGetTickCount，这两个函数切不可混用。 */
~~~

2. 函数 vTaskDelay 和 vTaskDelayUntil 的区别
函数 vTaskDelayUntil 实现的是周期性延迟(绝对性延时)，而函数 vTaskDelay 实现的是相对性延迟，反映到实际应用上有什么区别呢？
>举例
>运行条件：
>- 有一个 bsp_KeyScan 函数，这个函数处理时间大概耗时 2ms。
>- 有两个任务，一个任务 Task1 是用的 vTaskDelay 延迟，延迟 10ms，另一个任务 Task2 是用的vTaskDelayUntil 延迟，延迟 10ms。
>-  不考虑任务被抢占而造成的影响。
> 实际运行过程效果：
>- Task1：
bsp_KeyScan+ vTaskDelay (10) ---> bsp_KeyScan + vTaskDelay (10)
|----2ms + 10ms 为一个周期------| |----2ms + 10ms 为一个周期----|
这个就是相对性的含义
>- Task2：
bsp_KeyScan + vTaskDelayUntil ---------> bsp_KeyScan + vTaskDelayUntil
|----10ms 为一个周期（2ms 包含在 10ms 内）---| |----10ms 为一个周期------|
这就是周期性的含义

## 6. 通信与同步机制

这里经过自己的理解，现将通信与同步机制做一个比喻，方便初学者理解。
> 场景：A 和 B 相约做地铁去公司，A/B互相告知自己去公司的经历。

可能发生的情景：
 - 事件标志组：A告诉B，我经过了1，2，3，4，5等等站；（这里经过的站的数量就是时间标志组的事件标志的最大容量，不超过24个）
- 二值信号量：A告诉B说自己到没到公司（到或者没到），没有其他信息；（A/B只能告诉对方自己的一个二值状态，如：到/没到，没有其他信息告诉）
- 计数信号量：A告诉B说自己到没到公司（到或者没到），目前经过几站，没有其他信息；（A/B只能告诉对方自己的多个状态，如：到/没到/在路上/在哪站，没有其他信息告诉）
- 互斥信号量：（A与B之前都约C一起去公司，坐C的电动自行车去公司，C说明谁先到我这我带谁去），A到C处时告诉B我已经到C处了，其他什么话都没有。然后C不忍心只有等送A到公司后再去接B；（这里C每次只能载一个人的规则，防止了A/B发生人身冲突）
- 消息邮箱：A告诉B，我经过了1，2，3，4，5等等站，每站的到站时间，我在每站做了什么事等等信息，B收到后就说知道了；（这里A只能说一次，一次说完，没说完没有机会接着上一次说，要么就是等明天从来来过）
- 消息队列：A一直告诉B，我经过了哪站站，到站时间，我在这站做了什么事等等信息，B收到后就说知道了，如果A频繁的说，B听的不耐烦了，B说你慢点说我前面的还没来得及听；（这里A可以说多次，这个次数就是消息队列的容量，当然，A既然可以告诉很多消息，A也可以选择像事件标志组，信号量那样说部分信息）

### 6.1 事件标志组

事件标志组是实现多任务同步的有效机制之一。也许有不理解的初学者会问采用事件标志组多麻烦，搞个全局变量不是更简单？其实不然，在裸机编程时，使用全局变量的确比较方便，但是在加上 RTOS 后就是另一种情况了。 使用全局变量相比事件标志组主要有如下三个问题：
- 使用事件标志组可以让 RTOS 内核有效地管理任务，而全局变量是无法做到的，任务的超时等机制需要用户自己去实现。
- 使用了全局变量就要防止多任务的访问冲突，而使用事件标志组则处理好了这个问题，用户无需担心。
- 使用事件标志组可以有效地解决中断服务程序和任务之间的同步问题。

任务间事件标志组的实现是指各个任务之间使用事件标志组实现任务的通信或者同步机制。
下面我们来说说 FreeRTOS 中事件标志的实现，根据用户在 FreeRTOSConfig.h 文件中的配置：
~~~
 #define configUSE_16_BIT_TICKS 1
~~~
配置宏定义 configUSE_16_BIT_TICKS 为 1 时，每创建一个事件标志组，用户可以使用的事件标志是8 个。
~~~
 #define configUSE_16_BIT_TICKS 0
~~~
配置宏定义 configUSE_16_BIT_TICKS 为 0 时，每创建一个事件标志组，用户可以使用的事件标志是24 个。
上面说的 8 个和 24 个事件标志应该怎么理解呢？其实就是定义了一个 16 位变量，仅使用了低 8bit或者定义了一个 32 位变量，仅使用了低 24bit。每一个 bit 用 0 和 1 两种状态来代表事件标志。 反映到FreeRTOS 上就是将事件标志存储到了 EventBits_t 类型的变量中。
~~~
#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif
~~~
==TickType_t 数据类型可以是 16 位数或者 32 位数，这样就跟上面刚刚说的configUSE_16_BIT_TICKS 宏定义呼应上了。推荐配置宏定义 configUSE_16_BIT_TICKS为 0，即用户每创建一个事件标志组，有 24 个标志可以设置。==
FreeRTOS 任务间事件标志组的实现是指任务间使用事件标志。
![任务间事件标志组的实现](https://img-blog.csdnimg.cn/20181227114212980.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)

FreeRTOS 中断方式事件标志组的实现是指中断函数和 FreeRTOS 任务之间使用事件标志。

![中断方式时间标志组的实现](https://img-blog.csdnimg.cn/20181227114109735.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
 实际应用中，中断方式的消息机制要注意以下四个问题：
1. 中断函数的执行时间越短越好，防止其它低于这个中断优先级的异常不能得到及时响应。
2. 实际应用中，建议不要在中断中实现消息处理，用户可以在中断服务程序里面发送消息通知任务，在任务中实现消息处理，这样可以有效地保证中断服务程序的实时响应。同时此任务也需要设置为高优先级，以便退出中断函数后任务可以得到及时执行。
3. 中断服务程序中一定要调用专用于中断的事件标志设置函数，即以 FromISR 结尾的函数。
4. 在操作系统中实现中断服务程序与裸机编程的区别。
(1) 如果 FreeRTOS 工程的中断函数中没有调用 FreeRTOS 的事件标志组 API 函数，与裸机编程是一样的。
(2) 如果 FreeRTOS 工程的中断函数中调用了 FreeRTOS 的事件标志组的 API 函数，退出的时候要检测是否有高优先级任务就绪，如果有就绪的，需要在退出中断后进行任务切换，这点跟裸机编程稍有区别；
(3) 另外强烈推荐用户将 Cortex-M3 内核的 STM32F103 和 Cortex-M4 内核STM32F407，F429的 NVIC 优先级分组设置为 4,即：NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);这样中断优先级的管理将非常方便。
(4)用户要在 FreeRTOS 多任务开启前就设置好优先级分组，一旦设置好切记不可再修改。

使用如下 11 个函数可以实现 FreeRTOS 的事件标志组：
>- xEventGroupCreate()
>-  xEventGroupCreateStatic()
>- vEventGroupDelete()
>-  xEventGroupWaitBits()
>- xEventGroupSetBits()
>- xEventGroupSetBitsFromISR()
>- xEventGroupClearBits()
>- xEventGroupClearBitsFromISR()
>- xEventGroupGetBits()
>-  xEventGroupGetBitsFromISR()
>- xEventGroupSync()

重点介绍这四个函数：
~~~
EventGroupHandle_t xEventGroupCreate( void );
~~~
返回值，如果创建成功，此函数返回事件标志组的句柄，如果 FreeRTOSConfig.h 文件中定义的 heap空间不足会返回 NULL。
~~~
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, /* 事件标志组句柄 */
                                const EventBits_t uxBitsToSet ); /* 事件标志位设置 */
  ~~~

函数 xEventGroupSetBits 用于设置指定的事件标志位为 1。
第 1 个参数：是事件标志组句柄。
第 2 个参数：表示 24 个可设置的事件标志位，EventBits_t 是定义的 32 位变量，低 24 位用于事件标志设置。变量 uxBitsToSet 的低 24 位的某个位设置为 1，那么被设置的事件标志组的相应位就设置为 1。变量 uxBitsToSet 设置为 0 的位对事件标志相应位没有影响。比如设置变量 uxBitsToSet = 0x0003 就表示将事件标志的位 0 和位 1 设置为 1，其余位没有变化。
返回值：当前的事件标志组数值。
这个函数注意事项：
（1）使用前一定要保证事件标志组已经通过函数 xEventGroupCreate 创建了。
（2） 此函数是用于任务代码中调用的，故不可以在中断服务程序中调用此函数，中断服务程序中使用的是
xEventGroupSetBitsFromISR
（3） 用户通过参数 uxBitsToSet 设置的标志位并不一定会保留到此函数的返回值中，下面举两种情况：
>（a） 调用此函数的过程中，其它高优先级的任务就绪了，并且也修改了事件标志，此函数返回的事件标志位会发生变化。

>（b） 调用此函数的任务是一个低优先级任务，通过此函数设置了事件标志后，让一个等待此事件标志的高优先级任务就绪了，会立即切换到高优先级任务去执行，相应的事件标志位会被函数xEventGroupWaitBits 清除掉，等从高优先级任务返回到低优先级任务后，函数xEventGroupSetBits 的返回值已经被修改。

~~~
BaseType_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, /* 事件标志组句柄 */
					const EventBits_t uxBitsToSet, /* 事件标志位设置 */
					BaseType_t *pxHigherPriorityTaskWoken ); /* 高优先级任务是否被唤醒的状态保存 */
~~~
函数 xEventGroupSetBits 用于设置指定的事件标志位为 1。
- 第 1 个参数：是事件标志组句柄。
- 第 2 个参数：表示 24 个可设置的事件标志位，EventBits_t 是定义的 32 位变量，低 24 位用于事件标志设置。变量 uxBitsToSet 的低 24 位的某个位设置为 1，那么被设置的事件标志组的相应位就设置为 1。 变量 uxBitsToSet 设置为 0 的位对事件标志相应位没有影响。比如设置变量 uxBitsToSet = 0x0003 就表示将事件标志的位 0 和位 1 设置为 1，其余位没有变化。
- 第 3 个参数：用于保存是否有高优先级任务准备就绪。如果函数执行完毕后，此参数的数值是 pdTRUE，说明有高优先级任务要执行，否则没有。
返回值：如果消息成功发送给 daemon 任务（就是 FreeRTOS 的定时器任务）返回 pdPASS，否则返回 pdFAIL，另外 daemon 任务中的消息队列满了也会返回 pdFAIL。
- 这个函数注意事项：
（1）使用前一定要保证事件标志已经通过函数 xEventGroupCreate 创建了。同时要在 FreeRTOSConfig.h文件中使能如下三个宏定义：
~~~
#define INCLUDE_xEventGroupSetBitFromISR 1
#define configUSE_TIMERS 1
#define INCLUDE_xTimerPendFunctionCall 1
~~~
（2）函数 xEventGroupSetBitsFromISR 是用于中断服务程序中调用的，故不可以在任务代码中调用此函数，任务代码中使用的是 xEventGroupSetBits。
（3） 函数 xEventGroupSetBitsFromISR 对事件标志组的操作是不确定性操作，因为不知道当前有多少个任务在等待此事件标志。而 FreeRTOS 不允许在中断服务程序和临界段中执行不确定性操作。 为了不在中断服务程序中执行，就通过此函数给 FreeRTOS 的 daemon 任务（就是 FreeRTOS 的定时器任务）发送消息，在 daemon 任务中执行事件标志的置位操作。 同时也为了不在临界段中执行此不确定操作，将临界段改成由调度锁来完成。这样不确定性操作在中断服务程序和临界段中执行的问题就都得到解决了。
（4）由于函数 xEventGroupSetBitsFromISR 对事件标志的置位操作是在 daemon 任务里面执行的，如果想让置位操作立即生效，即让等此事件标志的任务能够得到及时执行，需要设置 daemon 任务的优先级高于使用此事件标志组的所有其它任务。
（5） 通过下面的使用举例重点一下函数 xEventGroupSetBitsFromISR 第三个参数的规范用法，初学者务必要注意。
~~~
static void TIM2_IRQHandler(void)
{
	BaseType_t xResult;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	/* 中断消息处理， 此处省略 */
	……
	/* 向任务 vTaskMsgPro 发送事件标志 */
	xResult = xEventGroupSetBitsFromISR(xCreatedEventGroup, /* 事件标志组句柄 */
										BIT_0 , /* 设置 bit0 */
										&xHigherPriorityTaskWoken );
	/* 消息被成功发出 */
	if( xResult != pdFAIL )
	{
		/* 如果 xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
~~~

~~~
EventBits_t xEventGroupWaitBits(const EventGroupHandle_t xEventGroup, /* 事件标志组句柄 */
				const EventBits_t uxBitsToWaitFor, /* 等待被设置的事件标志位 */
				const BaseType_t xClearOnExit, /* 选择是否清零被置位的事件标志位 */
				const BaseType_t xWaitForAllBits, /* 选择是否等待所有标志位都被设置 */
				TickType_t xTicksToWait ); /* 设置等待时间 */
~~~
函数 xEventGroupWaitBits 等待事件标志被设置。
- 第 1 个参数：是事件标志组句柄。
- 第 2 个参数：表示等待 24 个事件标志位中的指定标志，EventBits_t 是定义的 32 位变量，低 24 位用于事件标志设置。比如设置变量 uxBitsToWaitFor = 0x0003 就表示等待事件标志的位 0 和位 1 设置为 1。 此参数切不可设置为 0。
- 第 3 个参数：选择是否清除已经被置位的事件标志，如果这个参数设置为 pdTRUE，且函数xEventGroupWaitBits 在参数 xTicksToWait 设置的溢出时间内返回，那么相应被设置的事件标志位会被清零。 如果这个参数设置为 pdFALSE，对已经被设置的事件标志位没有影响。
- 第 4 个参数：选择是否等待所有的标志位都被设置，如果这个参数设置为 pdTRUE，要等待第 2 个参数 uxBitsToWaitFor 所指定的标志位全部被置 1，函数才可以返回。当然，超出了在参数xTicksToWait 设置的溢出时间也是会返回的。如果这个参数设置为 pdFALSE，第 2 个参数uxBitsToWaitFor 所指定的任何标志位被置 1，函数都会返回，超出溢出时间也会返回。
- 第 5 个参数：设置等待时间，单位时钟节拍周期。 如果设置为 portMAX_DELAY，表示永久等待。
返回值：由于设置的时间超时或者指定的事件标志位被置 1，导致函数退出时返回的事件标志组数值。
这个函数注意事项：
（1）此函数切不可在中断服务程序中调用。
（2）这里再着重说明下这个函数的返回值，通过返回值用户可以检测是哪个事件标志位被置 1 了。
>（a）如果由于设置的等待时间超时，函数的返回值可会有部分事件标志位被置 1。
（b）如果由于指定的事件标志位被置1而返回，并且设置了这个函数的参数xClearOnExit为pdTRUE，那么此函数的返回值是清零前的事件标志组数值。
另外，调用此函数的任务在离开阻塞状态到退出函数 xEventGroupWaitBits 之间这段时间，如果一个高优先级的任务抢占执行了，并且修改了事件标志位，那么此函数的返回值会跟当前的事件标志组数值不同。

### 6.2 定时器组
FreeRTOS支持的定时器组，或者叫软件定时器，又或者叫用户定时器均可。软件定时器的功能比较简单，也容易掌握。被称为定时器组是因为用户可以创建多个定时器，创建的个数是可配置的。
FreeRTOS软件定时器组的时基是基于系统时钟节拍实现的，之所以叫软件定时器是因为它的实现不需要使用任何硬件定时器，而且可以创建很多个，综合这些因素，这个功能就被称之为软件定时器组。 既然是定时器，那么它实现的功能与硬件定时器也是类似的。在硬件定时器中，我们是在定时器中断中实现需要的功能，而使用软件定时器时，我们是在创建软件定时器时指定软件定时器的回调函数，在回调函数中实现相应的功能。
FreeRTOS提供的软件定时器支持单次模式和周期性模式，单次模式就是用户创建了定时器并启动了定时器后，定时时间到将不再重新执行，这就是单次模式软件定时器的含义。周期模式就是此定时器会按照设置的时间周期重复去执行，这就是周期模式软件定时器的含义。另外就是单次模式或者周期模式的定时时间到后会调用定时器的回调函数，用户可以回调函数中加入需要执行的工程代码。
定时器任务（Daemon 任务）
为了更好的管理FreeRTOS的定时器组件，专门创建了一个定时器任务，或者称之为Daemon任务。关于这个任务，我们上章节在讲解事件标志组的时候有用到。
FreeRTOS定时器组的大部分API函数都是通过消息队列给定时器任务发消息，在定时器任务里面执行实际的操作。为了更好的说明这个问题，我们将官方在线版手册中的这个截图贴出来进行说明：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190604102438427.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
左侧图是用户应用程序，右侧是定时器任务。在用户应用程序里面调用了定时器组API函数xTimerReset，这个函数会通过消息队列给定时器任务发消息，在定时器任务里面执行实际操作。消息队列在此处的作用有一个专门的名字：Timer command queue，即专门发送定时器组命令的队列。
>**使用软件定时器组注意事项：**
> 定时器回调函数是在定时器任务中执行的，实际应用中切不可在定时器回调函数中调用任何将定时 器任务挂起的函数，比如vTaskDelay(),
> vTaskDelayUntil()以及非零延迟的消息队列和信号量相关的函数。将定时器任务挂起，会导致定时器任务负责的相关功能都不能正确执行了。
>
使用如下 20 个函数可以实现 FreeRTOS 的事件标志组：

 - xTimerCreate()  
 - xTimerCreateStatic()  
 - xTimerIsTimerActive()
 - xTimerStart()  
 - xTimerStop()   
 - xTimerChangePeriod()
 - xTimerDelete()
 - xTimerReset()
 - xTimerStartFromISR()
 - xTimerStopFromISR()    
 - xTimerChangePeriodFromISR()
 - xTimerResetFromISR()     
 - pvTimerGetTimerID()     
 - vTimerSetTimerID()     
 - xTimerGetTimerDaemonTaskHandle()
 -  xTimerPendFunctionCall()     
 - xTimerPendFunctionCallFromISR()      
 - pcTimerGetName()     
 - xTimerGetPeriod()     
 - xTimerGetExpiryTime()

重点介绍这3个函数：
~~~
xTimerCreate()
xTimerStart ()
pvTimerGetTimerID ()
~~~
函数 xTimerCreate

    TimerHandle_t xTimerCreate
    					( const char * const pcTimerName,               /* 定时器名字 */
    	  				const TickType_t xTimerPeriod,                /* 定时器周期，单位系统时钟节拍
    	  				const UBaseType_t uxAutoReload,     /* 选择单次模式或者周期模式 */
    	  				void * const pvTimerID,                       /* 定时器 ID */
    					TimerCallbackFunction_t pxCallbackFunction ); /* 定时器回调函数 */

函数描述： 函数xTimerCreate用于创建软件定时器。

 - 第1个参数是定时器名字，用于调试目的，方便识别不同的定时器。
 -  第2个参数是定时器周期，单位系统时钟节拍。
 - 第3个参数是选择周期模式还是单次模式，若参数为pdTRUE，则表示选择周期模式，若参数为pdFALSE，则表示选择单次模式。
 -   第4个参数是定时器ID，当创建不同的定时器，但使用相同的回调函数时，在回调函数中通过不同的ID号来区分不同的定时器。    
 - 第5个参数是定时器回调函数。
 - 返回值，创建成功返回定时器的句柄，由于FreeRTOSCongfig.h文件中heap空间不足，或者定时器周期设置为0，会返回NULL

函数 xTimerStart
函数原型：

    BaseType_t xTimerStart( TimerHandle_t xTimer,    /* 定时器句柄 */
    										TickType_t xBlockTime ); /* 成功启动定时器前的最大等待时间设置，单位系统时钟节拍 */


函数描述：函数xTimerStart用于启动软件定时器。

 - 第1个参数是定时器句柄。
 - 第2个参数是成功启动定时器前的最大等待时间设置，单位系统时钟节拍，定时器组的大部分API函数不是直接运行的，而是通过消息队列给定时器任务发消息来实现的，此参数设置的等待时间就是当消息队列已经满的情况下，等待消息队列有空间时的最大等待时间。
 - 返回值，返回pdFAIL表示此函数向消息队列发送消息失败，返回pdPASS表示此函数向消息队列发送消息成功。定时器任务实际执行消息队列发来的命令依赖于定时器任务的优先级，如果定时器任务是高优先级会及时得到执行，如果是低优先级，就要等待其余高优先级任务释放CPU权才可以得到执行。

> 使用这个函数要注意以下问题：
> 1. 使用前一定要保证定时器组已经通过函数xTimerCreate创建了。
> 2.  在FreeRTOSConfig.h文件中使能宏定义： #define configUSE_TIMERS 1
> 3. 对于已经被激活的定时器，即调用过函数xTimerStart进行启动，再次调用此函数相当于调用了函数xTimerReset对定时器时间进行了复位。
> 4. 如果在启动FreeRTOS调度器前调用了此函数，定时器是不会立即执行的，需要等到启动了FreeRTOS调度器才会得到执行，即从此刻开始计时，达到xTimerCreate中设置的单次或者周期性延迟时间才会执行相应的回调函数。


函数 pvTimerGetTimerID
函数原型：

    void *pvTimerGetTimerID( TimerHandle_t xTimer ); /* 定时器句柄 */      

函数描述： 函数pvTimerGetTimerID用于返回使用函数xTimerCreate创建的软件定时器ID。
- 第1个参数是定时器句柄。
-  返回值，返回定时器ID。

> 使用这个函数要注意以下问题：
> 1. 使用前一定要保证定时器组已经通过函数xTimerCreate创建了。
> 2. 在FreeRTOSConfig.h文件中使能宏定义： #define configUSE_TIMERS 1
> 3. 创建不同的定时器时，可以对定时器使用相同的回调函数，在回调函数中通过此函数获取是哪个定时器的时间到了，这个功能就是此函数的主要作用。

### 6.3 消息队列
#### [cubemx配置freertos的消息队列](https://github.com/sangeren1002/Notes/blob/master/FreeRTOS/queue.md)
>消息队列的概念及其作用
消息队列就是通过 RTOS 内核提供的服务，任务或中断服务子程序可以将一个消息（注意，FreeRTOS
消息队列传递的是实际数据，并不是数据地址，RTX，uCOS-II 和 uCOS-III 是传递的地址）放入到队列。
同样，一个或者多个任务可以通过 RTOS 内核服务从队列中得到消息。通常，先进入消息队列的消息先传
给任务，也就是说，任务先得到的是最先进入到消息队列的消息，即先进先出的原则（FIFO），FreeRTOS
的消息队列支持 FIFO 和 LIFO 两种数据存取方式。

*需要说明的是，freertos消息队列是通过副本机制传递的，而不是引用。*

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190704090814366.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
查看源码可以看到底层实现`xQueueSend`函数的宏定义如下
```
#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )

```
`xQueueGenericSend()`函数内有这个函数
```
xYieldRequired = prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );
```
`prvCopyDataToQueue()`函数内可以看到
```
( void ) memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); /*lint !e961 !e418 MISRA exception as the casts are only redundant for some ports, plus previous logic ensures a null pointer can only be passed to memcpy() if the copy size is 0. */
```
*freertos的消息队列发送函数是通过使用memcpy复制的内容。*
以简单的数据元素为例：
```
uint8_t ucCount = 0;

xQueueSend(xQueue1,(void *) &ucCount,(TickType_t)10);
```

这里是发送队列消息函数，下面看接收：
```
uint8_t ucQueueMsgValue;

xQueueReceive(xQueue1, /* 消息队列句柄 */
(void *)&ucQueueMsgValue, /* 存储接收到的数据到变量ucQueueMsgValue中 */
(TickType_t)xMaxBlockTime)
```

这里是最简单的uint_8类型元素，要想把发送函数的uint_8定义的数据，包括该数据在发送函数之前被更改后的值发送给接收函数，我们需要传递给发送函数send一个uint_8定义数据的地址，这样可以通过地址传递到memcpy函数，实现复制，这也就是为什么上面说的freertos的消息队列不是引用而是复制，要是引用的话，可以直接传这个uint_8类型的数据，而我们此时在freertos操作系统上，是副本传递，通过memcpy，所以需要给uint_8类型数据的地址。
同样的在本次实验中采用了消息队列传结构体指针地址来实现大数据量的传输提高效率。
```
/* 发送函数 */
MSG * TXMSG;
TXMSG = &DATAMSG;//DATAMSG是一个结构实体而且是全局区定义

	/* 初始化结构体内容 */
	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
/* 消息队列传递结构体指针的地址 */
  if(xQueueSend(myQueue02Handle,(void*)&TXMSG,0) ==errQUEUE_FULL)
		{
			printf("myQueue02Handle errQUEUE_FULL\r\n");
		}
```
```
/* 接收函数 */
  MSG *RXMSG;
	/* 初始化结构体内容 */
	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
/* 接收消息队列结构体指针的地址 */
if(xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10) == pdPASS)
{
  printf("\r\nRXMSG->ucMessageID = %d \r",RXMSG->ucMessageID);
  printf("RXMSG->ucData[0] = ");
  for(i=0;i<20;i++)
    printf(" %03d",RXMSG->ucData[i]);
  printf("\r\n");
  HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
}
```
这里的关键就在第二个参数ptMsg，它已经是指针了，为什么还要取地址，这样不是一个二级指针了吗，而它的参数是`void *`，给人的感觉应该就是传一个地址，虽然二级指针也是地址，但是总觉得不应该设计成二级指针赋值给一个一级指针，哪怕你是void*。但是我们既然使用了freertos，就要遵循别人的设计，别人这样做，肯定有自己的道理，我们做到熟练应用即可。试想，消息发送函数，要发送数据，要得到这个数据的地址以给memcpy，如果传递的数据本身就是地址（指针），那么我们要把这个地址传到接收函数去，就应该得到此时指针的地址才行，也就是传递一个指针的值，注意不是指针指向的值。关键我们要通过memcpy函数，传递一个指针的值通过memcpy必然是需要二级指针的，这样才可以操作一级指针的值，这样也就可以理解为什么ptMsg已经是指针了，却还是要传递ptMsg的地址，因为只有这样，才可以通过memcpy函数把ptMsg指针的值给到接收函数的指针，这样在接收函数中操作这个结构体类型的指针，就可以得到发送端的数据。这样做的好处是，避免了大数据的拷贝，只拷贝指针，提高了效率，但是使用指针，一定不要在栈空间开辟，这也是为什么我们定义g_tMsg结构体实体在全局区。但是freertos任务中一直有while（1），元素生命周期一直都在，此时还是可以使用局部变量做数据传递工具，但是这样的编程模式应该摒弃，我们采用全局区开辟的空间。

##### 1. 环境
- packages版本（STM32F4 1.21）
- cubemx版本（version4.27.0 && STM32Cube v1.0）
- MDK版本（KEIL6 V5.23.0.0）

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190704090713496.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190704090723954.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190704090732518.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
##### 2. cubemx配置
>主要内容：
1. 配置时钟
2. 配置串口
3. 使能freertos
4. 勾选任务信息相关宏定义（3个，方便查看任务信息）
5. 添加消息队列

STM32F07开发板实验，关键代码实现
```
/* cubemx自动生成的创建2个消息队列 */
/* 创建10个uint8_t型消息队列 */
  osMessageQDef(myQueue01, 10, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

/* 创建10个存储指针变量的消息队列 */
  osMessageQDef(myQueue02, 1, MSG*);
  myQueue02Handle = osMessageCreate(osMessageQ(myQueue02), NULL);
```
freertos.c源文件
```
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "bsp_usart.h"
#include "bsp_gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile unsigned long  ulHighFrequencyTimerTicks = 0ul;
typedef struct AMessage
{
    char ucMessageID;
    char ucData[ 20 ];
}MSG;

MSG DATAMSG;
/* USER CODE END Variables */
osThreadId PrintfTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osMessageQId myQueue01Handle;
osMessageQId myQueue02Handle;
osMessageQId myQueue03Handle;
osMessageQId myQueue04Handle;
osMessageQId myQueue05Handle;
osTimerId myTimer01Handle;
osTimerId myTimer02Handle;
osMutexId myMutex01Handle;
osSemaphoreId myBinarySem01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   void Show_SYS_INFO_Task(void);
/* USER CODE END FunctionPrototypes */

void StartPrintfTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void Callback01(void const * argument);
void Callback02(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
uint8_t buff[50] ={0x01,0x02,0x03} ;
/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
void configureTimerForRunTimeStats(void)
{
	ulHighFrequencyTimerTicks = 0ul;
}

unsigned long getRunTimeCounterValue(void)
{
	return ulHighFrequencyTimerTicks;
}

/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer02 */
  osTimerDef(myTimer02, Callback02);
  myTimer02Handle = osTimerCreate(osTimer(myTimer02), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of PrintfTask */
  osThreadDef(PrintfTask, StartPrintfTask, osPriorityNormal, 0, 512);
  PrintfTaskHandle = osThreadCreate(osThread(PrintfTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 256);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityNormal, 0, 256);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
/* what about the sizeof here??? cd native code */
  osMessageQDef(myQueue01, 10, uint8_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);
//	myQueue01Handle = xQueueCreate(10,4);
  /* definition and creation of myQueue02 */
/* what about the sizeof here??? cd native code */
  osMessageQDef(myQueue02, 1, MSG*);
  myQueue02Handle = osMessageCreate(osMessageQ(myQueue02), NULL);
////	myQueue02Handle = xQueueCreate(10,sizeof(struct AMessage*));
//	myQueue02Handle = xQueueCreate(10,sizeof(MSG *));

//  /* definition and creation of myQueue03 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue03, 32, uint8_t);
//  myQueue03Handle = osMessageCreate(osMessageQ(myQueue03), NULL);

//  /* definition and creation of myQueue04 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue04, 32, uint8_t);
//  myQueue04Handle = osMessageCreate(osMessageQ(myQueue04), NULL);

//  /* definition and creation of myQueue05 */
///* what about the sizeof here??? cd native code */
//  osMessageQDef(myQueue05, 32, uint8_t);
//  myQueue05Handle = osMessageCreate(osMessageQ(myQueue05), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartPrintfTask */
/**
  * @brief  Function implementing the PrintfTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartPrintfTask */
void StartPrintfTask(void const * argument)
{

  /* USER CODE BEGIN StartPrintfTask */

	MSG * TXMSG;
	TXMSG = &DATAMSG;
	uint8_t j = 0;
	uint8_t i;

	TXMSG->ucMessageID = 1;
	TXMSG->ucData[0] = 0;
  /* Infinite loop */
  for(;;)
  {  	
	if(key0_value == KEY0_PRES)
	{
		osSemaphoreRelease(myBinarySem01Handle);
		key0_value = 0;
	}
	if(key1_value == KEY1_PRES)
	{
		TXMSG->ucMessageID++;
		for(i=0;i<20;i++,j++)
		{
			TXMSG->ucData[i]=j;
		}
		if(xQueueSend(myQueue02Handle,(void*)&TXMSG,0) ==errQUEUE_FULL)
		{
			printf("myQueue02Handle errQUEUE_FULL\r\n");
		}
		key1_value = 0;
	}
	if(key2_value == KEY2_PRES)
	{
		if(xQueueSend(myQueue01Handle,(void*)&key2_value,10) == errQUEUE_FULL)
		{
			printf("myQueue01Handle errQUEUE_FULL\r\n");
		}
		key2_value = 0;
	}
	if(keyup_value == WKUP_PRES)
	{
		Show_SYS_INFO_Task();
		keyup_value = 0;
	}
	j++;
	if(j%10 == 0)
	  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

    osDelay(10);
  }
  /* USER CODE END StartPrintfTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {	 
	  osSemaphoreWait(myBinarySem01Handle, osWaitForever);
	  HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	  printf("StartTask02 osSemaphoreWait(myBinarySem01Handle);\r\n");     
	  osDelay(10);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
	uint8_t pvBuffer ;
	MSG *RXMSG;
	uint16_t i = 0;
  /* Infinite loop */
  for(;;)
  {
	if(xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10) == pdPASS)
	{
		printf("\r\nRXMSG->ucMessageID = %d \r",RXMSG->ucMessageID);
		printf("RXMSG->ucData[0] = ");
		for(i=0;i<20;i++)
			printf(" %03d",RXMSG->ucData[i]);
		printf("\r\n");
		HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	}
	if(xQueueReceive( myQueue01Handle, (void *) &pvBuffer, 10) == pdPASS)  
	{
		printf("myQueue01Handle %03d \r\n",pvBuffer);
		HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	}
	osDelay(10);
  }
  /* USER CODE END StartTask03 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */

  /* USER CODE END Callback01 */
}

/* Callback02 function */
void Callback02(void const * argument)
{
  /* USER CODE BEGIN Callback02 */

  /* USER CODE END Callback02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Show_SYS_INFO_Task(void)
{
	uint8_t pcWriteBuffer[200];
	printf("=================================================\r\n");
	printf("\r\ntask_name  \tstate\t prior\trtack\t Id\r\n");
	vTaskList((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

	printf("\r\ntask_name     time_count(10us) usage_pec\r\n");
	vTaskGetRunTimeStats((char *)&pcWriteBuffer);
	printf("%s\r\n", pcWriteBuffer);

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

```
FreeRTOSConfig.h头文件
```
/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */   	      
/* Section where include file can be added */
/* USER CODE END Includes */

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
/* USER CODE BEGIN 0 */   	      
    extern void configureTimerForRunTimeStats(void);
    extern unsigned long getRunTimeCounterValue(void);  
/* USER CODE END 0 */       
#endif

#define configUSE_PREEMPTION                     1
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
#define configTICK_RATE_HZ                       ((TickType_t)1000)
#define configMAX_PRIORITIES                     ( 7 )
#define configMINIMAL_STACK_SIZE                 ((uint16_t)64)
#define configTOTAL_HEAP_SIZE                    ((size_t)15360)
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configGENERATE_RUN_TIME_STATS            1
#define configUSE_TRACE_FACILITY                 1
#define configUSE_STATS_FORMATTING_FUNCTIONS     1
#define configUSE_16_BIT_TICKS                   0
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                8
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                ( 2 )
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             128

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_pcTaskGetTaskName           1
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_eTaskGetState               1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: This define MUST be commented when used with STM32Cube firmware,
              to prevent overwriting SysTick_Handler defined within STM32Cube HAL */
/* #define xPortSysTickHandler SysTick_Handler */

/* USER CODE BEGIN 2 */    
/* Definitions needed when configGENERATE_RUN_TIME_STATS is on */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS configureTimerForRunTimeStats
#define portGET_RUN_TIME_COUNTER_VALUE getRunTimeCounterValue    
/* USER CODE END 2 */

/* USER CODE BEGIN Defines */   	      
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */

#endif /* FREERTOS_CONFIG_H */

```
*FreeRTOS使用queue的注意事项*
1. FreeRTOS消息队列最大传递数据为一个32位值，且底层实现是memcpy，所以需要传递变量地址；如果只是简单的变量传输可以直接传递变量地址，`osMessageQDef(myQueue01, 32, uint8_t);`中的最后一个参数type可以为uint8_t或uint16_t或uint32_t等。
2. 如果想要传输多字节数据，我们只要将指向数据的地址的指针传递过去即可（指针本身大小为32位），如我们定义一个结构体指针`TXMSG`用来存放发送数据，数据完成打包后，使用`在xQueueSend(myQueue02Handle,(void*)&TXMSG,0)`将结构体指针的地址用消息队列发送出去。在消息队列接收端同样定义一个相同类型的结构体指针`RXMSG`来接收数据`xQueueReceive( myQueue02Handle, (void*)&RXMSG, 10)`,这样即可以实现多字节大量数据的传输。
```
typedef struct AMessage
{
    char ucMessageID;
    char ucData[ 20 ];
}MSG;
```
3. 多字节的大数据传输时，一定要在全局区定义并赋初始值以便程序运行时即可分配空间，如果在函数内部定义为局部变量（要传输的结构体指针），在函数结束时释放空间，则改地址可能被修改，在消息队列接收时出现数据错误。
>在定义接受和发送结构体指针时一定记得要赋初始值以便在在程序运行时即分配空间，否则易出现传输数据异常。
`
MSG DATAMSG;
MSG * TXMSG = &DATAMSG;
`

##### 串口打印效果：
![在这里插入图片描述](https://img-blog.csdnimg.cn/2019070409031376.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
### 6.4 计数信号量&二值信号量&互斥信号量
信号量（semaphores）是20世纪60年代中期Edgser Dijkstra发明的。使用信号量的最初目的是为了给共享资源建立一个标志，该标志表示该共享资源被占用情况。这样，当一个任务在访问共享资源之前，就可以先对这个标志进行查询，从而在了解资源被占用的情况之后，再来决定自己的行为。 实际的应用中，信号量的作用又该如何体现呢？比如有个30人的电脑机房，我们就可以创建信号量的初始化值是30，表示30个可用资源，不理解的初学者表示信号量还有初始值？是的，信号量说白了就是共享资源的数量。另外我们要求一个同学使用一台电脑，这样每有一个同学使用一台电脑，那么信号量的数值就减一，直到30台电脑都被占用，此时信号量的数值就是0。如果此时还有几个同学没有电脑可以使用，那么这几个同学就得等待，直到有同学离开。有一个同学离开，那么信号量的数值就加1，有两个就加2，依此类推。刚才没有电脑用的同学此时就有电脑可以用了，有几个同学用，信号量就减几，直到再次没有电脑可以用，这么一个过程就是使用信号量来管理共享资源的过程。

平时使用信号量主要实现以下两个功能：
- 两个任务之间或者中断函数跟任务之间的同步功能，这个和前面章节讲解的事件标志组是类似的。其实就是共享资源为1的时候。
-  多个共享资源的管理，就像上面举的机房上机的例子。 针对这两种功能，FreeRTOS分别提供了二值信号量和计数信号量，其中二值信号量可以理解成计数信号量的一种特殊形式，即初始化为仅有一个资源可以使用，只不过FreeRTOS对这两种都提供了API函数，而像RTX，uCOS-II和III是仅提供了一个信号量功能，设置不同的初始值就可以分别实现二值信号量和计数信号量。当然，FreeRTOS使用计数信号量也能够实现同样的效果。

==**三者的关系**==
二值信号量是计数信号量的特例，二值信号量与互斥信号量原理大同，不同点在互斥信号量可以避免多任务访问共享变量时出现的优先级反转。

==优先级反转==
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200226114108752.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3hpbmd6aGV3YW5mdQ==,size_16,color_FFFFFF,t_70)
运行过程描述如下：
- 任务Task1运行的过程需要调用函数printf，发现任务Task3正在调用，任务Task1会被挂起，等待Task3释放函数printf。
- 在调度器的作用下，任务Task3得到运行，Task3运行的过程中，由于任务Task2就绪，抢占了Task3的运行。优先级翻转问题就出在这里了，从任务执行的现象上看，任务Task1需要等待Task2执行完毕才有机会得到执行，这个与抢占式调度正好反了，正常情况下应该是高优先级任务抢占低优先级任务的执行，这里成了高优先级任务Task1等待低优先级任务Task2完成。所以这种情况被称之为==优先级翻转问题==。
-  任务Task2执行完毕后，任务Task3恢复执行，Task3释放互斥资源后，任务Task1得到互斥资源，从而可以继续执行。 上面就是一个产生优先级翻转问题的现象。

==**防止优先级翻转的过程**==

运行条件：
- 创建2个任务Task1和Task2，优先级分别为1和3，也就是任务Task2的优先级最高。
- 任务Task1和Task2互斥访问串口打印printf。
-  使用FreeRTOS的互斥信号量实现串口打印printf的互斥访问。

运行过程描述如下：
- 低优先级任务Task1执行过程中先获得互斥资源printf的执行。此时任务Task2抢占了任务Task1的执行，任务Task1被挂起。任务Task2得到执行。
-  任务Task2执行过程中也需要调用互斥资源，但是发现任务Task1正在访问，此时任务Task1的优先级会被提升到与Task2同一个优先级，也就是优先级3，这个就是所谓的==优先级继承==（Priority inheritance），这样就有效地防止了优先级翻转问题。任务Task2被挂起，任务Task1有新的优先级继续执行。
-  任务Task1执行完毕并释放互斥资源后，优先级恢复到原来的水平。由于互斥资源可以使用，任务 Task2获得互斥资源后开始执行。

### 6.5 任务计数信号量&任务二值信号量&任务事件标志组&任务消息邮箱
FreeRTOS计数信号量的另一种实现方式----基于任务通知（Task Notifications）的计数信号量，这里我们将这种方式实现的计数信号量称之为任务计数信号量。任务计数信号量效率更高，需要的RAM空间更小。当然，缺点也是有的，它没有前面介绍的计数信号量实现的功能全面。

任务通知的介绍
FreeRTOS每个已经创建的任务都有一个任务控制块（task control block），任务控制块就是一个结构体变量，用于记录任务的相关信息。结构体变量中有一个32位的变量成员ulNotifiedValue是专门用于任务通知的。 通过任务通知方式可以实现计数信号量，二值信号量，事件标志组和消息邮箱（消息邮箱就是消息队列长度为1的情况）。使用方法与前面章节讲解的事件标志组和信号量基本相同，只是换了不同的函数来实现。
任务通知方式实现的计数信号量，二值信号量，事件标志组和消息邮箱是通过修改变量ulNotifiedValue实现的：
- 设置接收任务控制块中的变量ulNotifiedValue可以实现消息邮箱。
- 如果接收任务控制块中的变量ulNotifiedValue还没有被其接收到，也可以用新数据覆盖原有数据 ，这就是覆盖方式的消息邮箱。  设置接收任务控制块中的变量ulNotifiedValue的bit0-bit31数值可以实现事件标志组。
- 设置接收任务控制块中的变量ulNotifiedValue数值进行加一或者减一操作可以实现计数信号量和二值信号量。

采用这种方式有什么优势呢？根据官方的测试数据，唤醒由于信号量和事件标志组而处于阻塞态的任务，速度提升了45%，而且这种方式需要的RAM空间更小。但这种方式实现的信号量和事件标志组也有它的局限性，主要表现在以下两个方面：
- 任务通知方式仅可以用在==只有一个任务等待信号量，消息邮箱或者事件标志组的情况==，不过实际项目项目中这种情况也是最多的。
-  使用任务通知方式实现的消息邮箱替代前面章节讲解的消息队列时，==发送消息的任务不支持超时等待==，即消息队列中的数据已经满了，可以等待消息队列有空间可以存新的数据，而任务通知方式实现的消息邮箱不支持超时等待。

**任务计数信号量/任务二值信号量/任务事件标志组消息邮箱 的源码追踪**

```c
osThreadDef(LED_Task, LED_TaskStart, osPriorityBelowNormal, 0, 256);
LED_TaskHandle = osThreadCreate(osThread(LED_Task), NULL);
	xTaskCreate((TaskFunction_t)thread_def->pthread,(const portCHAR *)thread_def->name,thread_def->stacksize, argument, makeFreeRtosPriority(thread_def->tpriority),&handle) != pdPASS)
		TCB_t *pxNewTCB;//这个就是任务控制块
			typedef tskTCB TCB_t;
				#if( configUSE_TASK_NOTIFICATIONS == 1 )
					volatile uint32_t ulNotifiedValue;
					volatile uint8_t ucNotifyState;
				#endif
```
