all: server client
LIBS = -lpthread
server: server.c
	gcc -g -W -Wall -o $@ $<

client: client.c
	gcc -W -Wall -o $@ $<

client: client.c
	gcc -W -Wall -o $@ $<

clean:
	rm server





#运行连接池前要执行	动态连接库的位置
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
#
#gcc test_db.c -I /usr/local/include/zdb -o test_db -lzdb

#src目录下使用
#gcc -I /root/Project1/include/ service_process.c server.c -o server -std=gnu99



