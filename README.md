### 需求原型
1.N个client通过某些嵌入式设备采集时间和位置信息，然后通过HTTP协议发送到Server端   
2.Server端接收来自每个Client的数据，通过解析数据提取核心信息，正确的信息存入数据库中并通知Client成功，错误的消息直接丢弃并通知Clinet失败  
3.数据库维护一个register table，决定某一编号的数据是否接收  
### 架构模型
![](https://github.com/LMingl/Project1_NetServer/blob/master/images/frame.png)

### 详细设计
流程图：
![](https://github.com/LMingl/Project1_NetServer/blob/master/images/flow1.png)
![](https://github.com/LMingl/Project1_NetServer/blob/master/images/flow2.png)


### 效果
1.硬件配置  
百度云 1核/2GB/40GB/计算型c2  
Linux version 3.10.0-1062.4.1.el7.x86_64      
mysql 5.6.47   
2.服务器启动    
![](images\运行.png)

3.jmeter并发发送数据   

4.数据落库   
![]()

5.聚合分析   





### 学习笔记&总结


### 参考文献

PS:






