### 需求原型
1.N个client通过某些嵌入式设备采集时间和位置信息，然后通过HTTP协议发送到Server端   
2.Server端接收来自每个Client的数据，通过解析数据提取核心信息，正确的信息存入数据库中并通知Client成功，错误的消息直接丢弃并通知Clinet失败  
3.数据库维护一个register table，决定某一编号的数据是否接收  
### 架构模型
![](https://github.com/LMingl/Project1_NetServer/blob/master/images/frame.png)

### 详细设计
流程图：  
![](https://github.com/LMingl/Project1_NetServer/blob/master/images/flow1.png)  

<img src="https://github.com/LMingl/Project1_NetServer/blob/master/images/flow2.jpg" width = "800" height = "700" alt="图片名称" align=center />

### 效果
1.硬件配置  
百度云 1核/2GB/40GB/计算型c2  
Linux version 3.10.0-1062.4.1.el7.x86_64      
mysql 5.6.47   
2.服务器启动       
<img src="https://github.com/LMingl/Project1_NetServer/blob/master/images/运行.png" width = "750" height = "350" alt="图片名称" align=center />    
3.数据落库   
<img src="https://github.com/LMingl/Project1_NetServer/blob/master/images/db.png" width = "750" height = "350" alt="图片名称" align=center />     
4.聚合分析   
<img src="https://github.com/LMingl/Project1_NetServer/blob/master/images/report.png" width = "750" height = "350" alt="图片名称" align=center />       
从图可以看出并发量并不高，因为是单核cpu无法发挥出线程池的作用

### 学习笔记&总结
<a href="https://blog.csdn.net/abc_lml" target="_blank">我的博客</a>记录了我在项目中遇到的问题及解决办法           
<a href="https://github.com/LMingl/Project1_NetServer/tree/master/笔记" target="_blank">我的笔记</a>一些知识的汇总

### 参考文献
Linux 高性能服务器编程/游双著.——北京：机械工业出版社，2013.5        
TCP/IP详解卷1：协议        
The C10K problem        

PS:如果图片不能正确加载，请参照这篇<a href="https://blog.csdn.net/weixin_42128813/article/details/102915578" target="_blank">博客</a>修改






