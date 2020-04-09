
oj = threadpool.o service_process.o database_process.o
zdb = /usr/local/
sv = /root/Project1/include
VPATH = $(zdb):$(sv):/usr/local/lib
all: client server

client: client.c
	gcc -g -W -Wall -o $@ $<
	

server: $(oj)
	gcc -o $@ $(oj) 

threadpool.o: threadpool.c
	gcc -c -g -I $(sv) $< -pthread -std=gnu99

service_process.o:service_process.c
	gcc -c -g $< 

database_process.o: database_process.c 
	gcc -c  $< -lzdb

.PHONY : clean

clean:
	-rm server $(oj) 




#运行连接池前要执行	动态连接库的位置
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
#
#gcc test_db.c -I /usr/local/include/zdb -o test_db -lzdb

#src目录下使用
#gcc -I /root/Project1/include/ service_process.c server.c -o server -std=gnu99


# gcc -I /root/Project1/include/ -I /usr/local/include/zdb/ database_process.c service_process.c server.c -o server -std=gnu99 -lzdb




#最终版本
#gcc -I /root/Project1/include/ -I /usr/local/include/zdb/ database_process.c threadpool.c service_process.c server.c -o server -std=gnu99 -lzdb -pthread

























