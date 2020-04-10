`tracks``tracks`#--------------------------------------------#
#**常见命令：**
#--------------------------------------------#
#1.查看当前所有的数据库
SHOW DATABASES;
#2.打开指定的库
USE 库名；
#3.查看当前库的所有表
SHOW TABLES;
#4.查看其他库的所有表
SHOW TABLES FROM 库名;
#5.创建表
CREATE TABLES 表名{
    列名 列类型，
    列名 列类型
}
#6.查看表结构
DESC 表名；

#-----------------------------------------------#
#**基础查询**
#-----------------------------------------------#
#语法：
#select 查询列表 from 表名；
#特点：
#1.查询列表可以是：表中的字段、常量值、表达式、函数
#2.查询的结果是一个虚拟的表格
#3.查询表中的多个字段（f12格式化）
#4.查询表中的所有字段 *  
#5.查询常量值
#6.查询表达式
SELECT department_id FROM departments;
SELECT job_id, job_title, min_salary FROM jobs;
SELECT `department_name`,`manager_id`,`location_id` FROM departments;
SELECT * FROM locations;
USE myemployees;
#6.查询函数
SELECT VERSION();
#7.起别名     1》便于理解  2》便于区分
SELECT 100%98 AS 结果；
SELECT manager_id AS 领导,location_id AS 位置 FROM departments;
SELECT salary AS "out put" FROM employees;
#8.去重
SELECT DISTINCT department_id FROM employees;
#9.加号 + 作用
/*
mysql中的加号只有一个作用：运算符
select 12+13;
select '123'+10;  其中一方为字符型，试图将字符型数值转换称数值型，转化成功继续做加法运算
select 'joi'+10;  转化失败，字符型数值转化成0
select null +10;   null和任何字段拼接都为null

*/
#将性和名连接成一个字段
SELECT CONCAT(first_name,last_name) AS 姓名 FROM employees;
#-----------------------------------------------------#
#**条件查询**
#-----------------------------------------------------#
/*
语法：select 查询列表 from 表名   where 筛选条件；
分类：
     一：按条件表达式筛选        < > = <>(不等于) <= >=
				安全等于 <=>
     
     二：按逻辑表达式查询     用于连接条件表达式
                                 and or not && || !
                                 
     三：模糊查询             一般和通配符配合使用
                              % 任意多个字符
                              _任意单个字符
                              \转义字符
                                 like 
                                 between and  包含临界值  不能颠倒顺序
                                 in    判断字段是不是属于列表中
                                       类型要一致
                                       不能用通配符省略
                                 is null
                                 is not null
                                 
                             
*/
SELECT * FROM employees WHERE salary > 12000;

SELECT department_id, first_name, last_name FROM employees WHERE department_id != 90;

SELECT 
	last_name, 
	salary, 
	commission_pct 
FROM 
	employees 
WHERE 
	salary > 10000 && salary < 20000

SELECT 
	*
FROM 
	employees
WHERE 
	department_id<90 OR department_id>110 OR salary>15000
#案例：查询的员工名中包含字符a的员工信息
SELECT
	*
FROM 
	employees
WHERE
	last_name LIKE '%a%';
#案例：查询员工名中第三个字符为e,第五个字符为a的员工名和工资
SELECT
	last_name,
	salary
FROM 
	employees
WHERE
	last_name LIKE '__e_a%';

#案例：查询员工名中第二个字符为_的员工名
SELECT
	last_name
FROM 
	employees
WHERE
	last_name LIKE "_\_%";
	
SELECT
	last_name
FROM
	employees
WHERE
	last_name LIKE '_$_%' ESCAPE '$';
#案例：查询员工编号在100到120之间的员工信息
SELECT 
	*
FROM 
	employees
WHERE
	employee_id BETWEEN 100 AND 120;   
SELECT
	*
FROM 
	employees
WHERE
	job_id IN ('IT_PROT','AD_VP','AD_PRES');
#案例：查询没有奖金的员工名和奖金率
SELECT
	last_name,
	commission_pct
FROM
	employees
WHERE
	commission_pct IS NULL;
SELECT
	last_name,
	commission_pct
FROM
	employees
WHERE
	commission_pct IS NOT NULL;	
#案例：查询员工号为176的员工的姓名和部门号和年薪
SELECT 
		last_name,
		department_id,
		salary * 12 *(1+IFNULL (commission_pct, 0)) AS 年薪
FROM
	employees
WHERE
	employee_id = 176;
	
#------------------------------------------------------
#排序查询
#------------------------------------------------------
/*
语法：
     select 查询列表
     from   表
     【where】
     order by 排序列表  asc(升序ascending order) | desc（降序 descending order）
特点：
     1.如果不写默认升序
     2.order by 子句中支持单个字段、多个字段、表达式、函数、别名
     3.order by 一般是放在最后，limit除外
     4.
*/
SELECT * FROM employees ORDER BY salary ASC;
SELECT * FROM employees ORDER BY salary DESC;	

SELECT * FROM employees WHERE department_id >= 90 ORDER BY hiredate ASC;

SELECT *,salary * 12 *(1+IFNULL(commission_pct, 0))AS 年薪 FROM employees 
ORDER BY 
	salary * 12 *(1+IFNULL(commission_pct, 0))
ASC;

SELECT *,salary * 12 *(1+IFNULL(commission_pct,0)) AS 年薪 FROM employees
ORDER BY 年薪 ASC;
#查询员工信息，要求先按工资升序，再按员工编号降序
SELECT * FROM employees 
ORDER BY salary ASC, employee_id DESC;
#-------------------------------------------------
#常见函数
#-------------------------------------------------
/*
功能：将一组逻辑语句封装在方法中，对外暴漏方法名
好处1.隐藏了实现方法
调用：select 函数名（实参列表）【from 表】
特点：
分类：
	1.单行函数
	如ifnull
	2.分组函数
	功能：做统计使用，又称统计函数、聚合函数
	
	字符函数 
		length
		concat 拼接字符串
		upper、lower 
		substr、substring  索引从1开始
		instr
		trim
		lpad rpad 用指定的字符实现左(右)填充指定长度
		replace 替换
	数学函数
		round 四舍五入  小数点控制
		ceil 向上取整
		floor 向下取整
		truncate 截断
		mod   取余
	日期函数
		now 返回当前系统的日期和时间
		curdate 返回当前系统日期，不包含时间
		curtime 返回当前时间，不包括日期
		year month day hour minter second  获取指定的部分 年 月 日 小时 分钟 秒
                str_to_date:将字符通过指定的格式转换成日期
                date_format: 将日期转换成字符		
		         格式符 %Y  四位的年份
		                %y  两位的年份
		                %m   月份01、02、03
		                %c   月份1、2、3、
		                %d   日 01、02、03
		                %H   小时（24小时制）
		                %h    小时(12小时制)
		                %i    分钟（00、01  ）
		                %s    秒
		                
	其他函数
		version
		datebase
		user
	流程控制函数
		if函数
		case函数：使用1      
			      case 要判断的字段和表达式
			      when 常量1 then 要显示的值1或语句
			      when 常量2 then 要显示的值2或语句
			      -----
			      else 要显示的值n或语句n
			      end
			  使用2
			      case 
			      when 条件1 then 要显示的值1或（语句1；）
			      when 条件2 then 要显示的值2或（语句2；）
			      else 要显示的值n或(语句n;)
			      end
	2.分组函数
	功能：做统计使用，又称统计函数、聚合函数
	sum() avg() max()  min()  count()
	特点：
             >sum avg只处理数值型，min max count处理任何类型
	     >分组函数都忽略null
	     >可以和distinct搭配实现去重
	     >一般使用count(*)用作统计行数
	     >和分组函数一同查询的字段要求是group by后的字段
	     
*/ 
SELECT LENGTH('str');
SELECT UPPER('str');
SELECT CONCAT(UPPER(last_name),'_', first_name)AS 姓名 FROM employees;
#索引从1开始
SELECT SUBSTR('大坏蛋在这里',4) out_out;
#截取从指定索引处指定字符长度的字符
SELECT SUBSTR('大坏蛋在这里',1,3) out_out;

SELECT CONCAT(UPPER(SUBSTR(last_name,1,1)),LOWER(SUBSTR(last_name,2))) AS 姓名 FROM employees;
#instr   返回字串在主串的位置
SELECT INSTR('你在哪里','呀') out_put;
#trim 去空格
SELECT TRIM('   你  在 ');
#lpad  用指定的字符实现左填充指定长度
SELECT LPAD('大牛',3,'*');
#replace 替换
SELECT
  REPLACE(
    '我吃饭了，你吃饭了没',
    '我',
    '你'
  );
# round

SELECT ROUND(-1.3);
SELECT ROUND(1.26,1); #保留一位小数
SELECT TRUNCATE(1.345,1);
SELECT MOD(-3,2);
#now
SELECT NOW();
SELECT CURDATE();
SELECT CURTIME();
SELECT SECOND(NOW());
SELECT * FROM employees WHERE hiredate = STR_TO_DATE('3-4-1992','%d-%c-%Y');
SELECT STR_TO_DATE("3-4-1992 21:12:12",'%d-%c-%Y %H:%i:%s');
#data_format
SELECT DATE_FORMAT(NOW(),'%Y年%m月%d日');
SELECT USER();
#case
SELECT
  salary 原工资,
  department_id,
  CASE
    department_id
    WHEN 30
    THEN salary * 1.1
    WHEN 40
    THEN salary * 1.2
    ELSE salary
  END 新工资
FROM
  employees;
#case用法2 查询员工工资情况
/*
如果工资>20000 显示级别A
如果工资>15000 显示级别B
如果工资>10000 显示级别C
其他显示D级别
*/
SELECT salary,last_name,
CASE
WHEN salary>20000 THEN 'A'
WHEN 20000>salary>15000 THEN 'B'
WHEN 15000>salary>10000 THEN 'C'
ELSE 'D'
END
级别
FROM employees;
#分组函数
SELECT SUM(salary) FROM employees;


SELECT MAX(salary) FROM employees;
#distinct去重
SELECT COUNT(DISTINCT salary) FROM employees;
#count
SELECT COUNT(*) FROM employees;
SELECT COUNT(*) FROM employees WHERE department_id = 90;
#-----------------------------------------------------
#分组查询
#----------------------------------------------------
/*
group by 将表中的数据分成若干组
语法：
	select 分组函数，列（要求出现在group by后面）
	from 表
	【where 筛选条件】
	group by 分组的列表
	【order by 子句】
注意：
	查询列表必须特殊，要求是分组函数和group by后出现的字段
特点     
	1.分组查询中的筛选条件分为两类
	分组前筛选   从原始表中           group by 之前   使用where
	分组后筛选   从分组后的结果集     group by 之后   使用having
	
	分组函数做条件肯定是用having
	2.group by 支持单个字段分组，多个字段分组（用逗号隔开，无顺序），表达式或函数
	3.可以添加排序，位置在整个分组最后
	
*/
#查询每个工种的最高工资
SELECT MAX(salary),job_id FROM employees GROUP BY job_id;
#查询每个位置的部门个数
SELECT COUNT(department_id),location_id FROM departments GROUP BY location_id;
#添加筛选条件
#查询邮箱中包含a字符的，每个部门的平均工资
SELECT AVG(salary),department_id 
FROM employees
WHERE email LIKE '%a%'
GROUP BY department_id;
#查询有奖金的每个领导手下员工的最高工资
SELECT MAX(salary),manager_id
FROM employees
WHERE commission_pct IS NOT NULL
GROUP BY manager_id;
#添加复杂的筛选条件
#查询每个部门的员工个数
SELECT COUNT(*),department_id
FROM employees
GROUP BY department_id;
#添加分组后的筛选，从生成的表中进行查询
SELECT COUNT(*),department_id
FROM employees
GROUP BY department_id
HAVING COUNT(*)>2;
#查询每个工种有奖金的员工的最高工资>12000的工种编号和最高工资
SELECT MAX(salary), job_id
FROM employees
WHERE commission_pct IS NOT NULL
GROUP BY job_id
HAVING MAX(salary)>12000;
#
SELECT MIN(salary), manager_id
FROM employees
WHERE manager_id > 102
GROUP BY manager_id
HAVING MIN(salary) > 5000;
#按表达式或函数分组
#按员工姓名的长度进行分组，查询每一组员工个数，筛选员工个数》5的有哪些
SELECT COUNT(*),LENGTH(last_name)
FROM employees
GROUP BY LENGTH(last_name)
HAVING COUNT(*)>5;
#按多个字段分组
#查询每个部门每个工种的员工的平均工资
SELECT AVG(salary),department_id,job_id
FROM employees
GROUP BY department_id,job_id;
#添加排序
#查询每个部门每个工种的员工的平均工资，并按平均工资的高低显示
SELECT AVG(salary),department_id,job_id
FROM employees
GROUP BY department_id,job_id
HAVING AVG(salary) > 10000
ORDER BY   AVG(salary)  DESC;
#
SELECT MAX(salary),MIN(salary),SUM(salary),job_id
FROM employees
GROUP BY job_id
ORDER BY job_id;
#-------------------------------------
#连接查询，又称多表查询
#-------------------------------------
/*
添加连接条件

分类：
	按年代分类：
		sql92
		sql99{推荐}
	按功能分类：
	    sql99支持：
	    	语法：
			select 查询列表
			from 表1  别名 
			【连接类型】join 表2  别名
			on  连接条件
			【where 筛选条件】
			【group by 】
			【having 】
			【order by】
		内连接：inner
			等值连接
			非等值连接
			自连接sehng
		   特点：
			inner可以省略
		外连接：
			左外连接 left 【outer】
			右外连接 right 【outer】
			全外连接（mysql不支持）full outer
		   特点：
			1.外连接的查询结果为主表中的所有记录
			    如果从表中有和它匹配的，则显示匹配的值
			    如果从表中没有和它匹配的，则显示null
			2.左外连接，left join 左边的是主表
			  右外连接，right join 右边是主表
			3.全外连接=内连接的结果+表1中有但表二没有+表2有表一没有
			4.交叉连接=笛卡尔乘积
		
		交叉连接 cross
	
		
		



*/
#员工名和对应的部门名
SELECT last_name,department_name
FROM employees,departments
WHERE employees.`department_id` = departments.`department_id`;
#查询员工名、工种号、工种名
#用表限定，防止歧义
#为表起别名，注意：如果为表起了别名，就不能再用原来的表名了
SELECT last_name, employees.job_id,job_title
FROM employees,jobs
WHERE employees.`job_id` = jobs.`job_id`;
#查询有奖金的员工名、部门名
SELECT last_name, employees.department_id
FROM employees,departments
WHERE employees.`department_id` = departments.`department_id`
AND employees.commission_pct IS NOT NULL;
#非等值连接

#自连接
#查询员工名和上级的名称
SELECT e.last_name, m.last_name
FROM employees e, employees m
WHERE e.`manager_id` = m.`employee_id`;
#sql99
#案例 查询员工名，部门名（连接位置）
SELECT last_name,department_name
FROM employees e
INNER JOIN departments d
ON e.`department_id` = d.`department_id`;
#查询名字中包括e的员工名和工种名
SELECT
  last_name,
  job_title
FROM
  employees e
  INNER JOIN jobs j
    ON e.`job_id` = j.`job_id`
WHERE e.`last_name` LIKE '%e%';
#查询员工名、部门名、工种名，并按部门名排序
#三表连接
SELECT 
	last_name,
	department_name,
	job_title
FROM 
	employees e
	INNER JOIN departments d
		ON e.`department_id` = d.`department_id` 
	INNER JOIN jobs j
		ON e.`job_id` = j.`job_id`
ORDER BY d.`department_name` DESC;
#查询那个部门没有员工
SELECT e.*,department_name
FROM employees e
RIGHT OUTER JOIN departments d
ON e.`department_id` = d.`department_id`
WHERE e.`employee_id` IS NULL;
#----------------------------------------------
#子查询
#-----------------------------------------------------
/*
定义：出现在其他语句中的select语句，称为子查询或内查询
分类：
按子查询出现的位置
	select 后面 标量
	from 后面 表
	where 或having后面
                    标量
                    列	
	exits后面
	        表
按结果集的行列数
	标量子查询（结果集只有一行一列）
	列子查询（结果集只有一列多行）
	行子查询（结果集有一行多列）
	表子查询（多行多列）
	
一、where/having后面
特点：
1.子查询放在小括号内
2.子查询一般放在条件的右侧
3.标量子查询，一般搭配着单行操作符使用
  > < = <> 
  列子查询,一般搭配着多行操作符使用
  in any/some all
二、select 后面
三、from 后面
结果集充当一张表，要求必须起别名
四、exists后面（相关子查询）
exists(完整的查询语句)
关心有没有

*/
#谁的工资比abel高
SELECT
  *
FROM
  employees
WHERE salary >
  (SELECT
    salary
  FROM
    employees
  WHERE last_name = 'Abel');
#
SELECT last_name, job_id, salary
FROM employees
WHERE job_id =
      (SELECT job_id
	FROM employees
	WHERE employee_id = 141
      )


AND
    salary >
    (SELECT salary
     FROM employees
     WHERE employee_id = 143
    );
#
SELECT last_name
FROM employees
WHERE salary = (
	SELECT MIN(salary)
	FROM employees
	

);
#having
SELECT MIN(salary)
FROM employees
WHERE department_id = 50;

SELECT MIN(salary),department_id
FROM employees
GROUP BY department_id
HAVING MIN(salary) > 
(SELECT MIN(salary)
FROM employees
WHERE department_id = 50);
#----------------------------------------------
#分页查询
#-------------------------------------------------
/*
应用场景：当要显示的数据，一页显示不全，需要分页提交
sql请求
语法：
	select 查询列表  7
	from 表  1
	join 表  2
	on 连接条件  3
	where 筛选条件  4
	gourp by 分组字段 5
	having 分组后筛选 6
	order by 排序字段   8
	limit offset,size;  9
	
	offset要显示条目的起始索引（起始索引从0开始）
	size要显示的条目个数
特点：
	1.limit语句放在查询语句的最后
        2. 公式
	
	
*/
#查询第11条-第25条
SELECT * FROM employees LIMIT 10,15;
#有奖金的员工信息，并且工资较高的前10名显示出来
SELECT *
FROM employees
WHERE commission_pct IS NOT NULL
ORDER BY salary DESC
LIMIT 0,10;
#--------------------------------------
#联合查询
#--------------------------------------------
/*
union 联合 合并：将多条查询语句的结果合并成一个结果

语法：
查询语句1
union 
查询语句2
union
查询语句3
-------

应用场景：
要查询的结果来自多个表，且多个表没有直接的连接关系，但查询的信息一致时

特点：
1.要求多条查询语句的查询列数是一致的
2.要求多条查询语句的查询的每一列的类型和顺序最好一致
3.union关键字默认去重，如果使用union all 可以包含重复项

*/
#-----------------------------------------
#-------------------------------------------
#DML语言
#--------------------------------------------
#--------------------------------------------
/*
数据操作语言：
插入：insert
修改：update
删除：delete

*/
#insert语句
/*
一、插入语句
语法：
方式一：
insert into 表名（列名，-----）values(值1，-------)
特点：
1.插入的值的类型要与列的类型一致或兼容
2.不可以为null的列必须插入值，可以为null的列插入null或不写列名
3.列的顺序可以颠倒
4.列数和值的个数必须一致
5.可以省略列名，默认所有列，而且列的顺序和表中列的顺序一致
6.可以插入多行
7.支持子查询

方式二：
语法：
insert into 表名
set 列名=值，列名=值，-------
不可以插入多行
不支持子查询
*/
#update语句
/*
1.修改单表的记录
语法：
update 表名
set 列=新值，列=新值，-----
where 筛选条件；

2.修改多表的记录

sql99语法：
update 表1 别名
inner|left|right join 表2 别名
on 连接条件
set 列=值，----
where 筛选条件；

*/
#删除语句
/*
方式一：delete
语法：
1.单表的删除
delete from 表名 where 筛选条件

2.多表的删除

方式二：truncate
语法：truncate table 表名；


*/
#------------------------------------
#DDL语言
#------------------------------------
/*
数据定义语言

库和表的管理

一、库的管理
创建、修改、删除
二、表的管理
创建、修改、删除

创建：create
修改：alter
删除：drop

*/
#一、库的管理
#1.库的创建
/*
语法：
create datebase [if not exists]库名；

*/
#创建库books
CREATE DATABASE IF NOT EXISTS books;
#2.库的修改
一般不使用
#更改库的字符集
ALTER DATABASE books CHARACTER SET utf8;
#3.库的删除
DROP DATABASE IF EXISTS books;

#二、表的管理
#1.表的创建
/*
语法：
create table 表名(
	列名 列的类型 【（长度）约束】，
	列名 列的类型 【（长度）约束】，
	-----------
	列名 列的类型 【（长度）约束】


)
*/
#创建表Book
CREATE TABLE IF NOT EXISTS book(
	id INT,#编号
	bName VARCHAR(20),#图书名
	price DOUBLE,#价格
	authorId INT,#作者编号
	publishDATE DATETIME #出版日期
);
#创建表author
CREATE TABLE author(
	id INT,
	au_name VARCHAR(20),
	nation VARCHAR(20)
);
#2.表的修改
/*
alter table 表名 add|drop|modify|chage colume 列名【类型 约束】

*/
#修改列名
ALTER TABLE book CHANGE COLUMN publishDATE pubDate DATETIME;
#修改列的类型或约束
ALTER TABLE book MODIFY COLUMN pubDate TIMESTAMP;
#添加新列
ALTER TABLE book ADD COLUMN annual DOUBLE;
#删除列
ALTER TABLE book DROP COLUMN annual;
#修改表名
ALTER TABLE author RENAME TO book_author;
#3.表的删除
DROP TABLE IF EXISTS book_author;

#4.表的复制

#仅仅复制表的结构
CREATE TABLE copy LIKE author;

#复制表的结构外加数据
CREATE TABLE copy2
SELECT * FROM author;
#只复制部分数据
CREATE TABLE copy3
SELECT id, au_name
FROM author
WHERE nation='中国';
#仅复制部分字段
CREATE TABLE copy4
SELECT id,au_name
FROM author
WHERE 0;
#---------------------------------
#常见的数据类型
#----------------------------------
/*
数值型：
	整性：
		tinyint smallint mediumint int bigint
		 1        2        3        4    5
	     特点：1>如果不设置有符号还是无符号，默认是有符号的，如果想设置无符号，需要添加unsigned
		   2>如果插入的数值超出了整性的范围，会报out of range异常，并且插入临界值
		   3>如果不设置长度，会有默认的长度
	
	小数： 浮点型：float(M,D) double(M,D)
	       定点型：dec(M,D)  decimal(M,D)
	       特点：1>M：整数部位+小数部   D:小数部位
		     2>M\D都可以省略  如果是decimal，则M默认为10，D默认为0
		     如果是float和double，则会根据插入的数值的精度来决定
		     3>定点数的精度较高
字符型：
	较短文本：char(M)固定字长 varchar(M)可变字长
	
	较长文本：text blob
	其他：binary  
	      enum 保存枚举
	      set 保存集合
	特点：1>M为最大的字符数 
	        char 比较耗费空间 但效率较高
	        varchar 不可省略

日期型：
	类型 date datetime timestamp time year
	字节   4     8         4       3     1
	
	特点：datatime不受时区影响 timestamp受时区影响


*/
#整形

#1.如何设置无符号和有符号
USE books;
DROP TABLE IF EXISTS int_tab;
CREATE TABLE int_tab(
	t1 INT(7),
	t2 INT UNSIGNED
);
INSERT INTO int_tab (t1,t2) VALUES(-2344, -34353255);


CREATE DATABASE IF NOT EXISTS tracks;
CREATE TABLE IF NOT EXISTS track(
	numb INT,#编号
	re_time DATETIME,#时间
	latitude DOUBLE,#经度
	longitude DOUBLE,#维度
	scale INT #缩放比
);
#常见约束
/*
含义：一种限制，用于限制表中的数据，为了保证表中的数据的准确和可靠性
分类：六大约束
	not null: 非空，保证该字段的值不能为空
	default:默认，用于保证该字段有默认值
	primary key:主键，用于保证该字段的值具有唯一性, 至多一个，允许组合但不推荐
	unique:唯一约束，可以为空 可以有多个  允许组合
	check:检查约束【mysql不支持】
	foreign key:外键 ，用于限制两个表的关系，用于保证该字段的值必须来自主表的关联列表
添加约束的时机：
	1.创建表时
	2.修改表时
约束的添加分类
	列级约束
		六大约束语法都支持，但外键约束没有效果
	表级约束
		除了非空、默认、，其他都支持
		
		P128
外键：
	1.要求在从表设置外键关系
	2.从表的外键列的类型和主表的关联列的类型要求一致或兼容，名称无要求
	3.主表的关联列必须是一个key（一般是主键或唯一）
	4.插入数据时，先插入主表，再插入从表
	删除数据时，先删除从表，再删除主表

   约束可以同时添加多个
*/
/*
#列级约束
*/
CREATE TABLE stuinfo{
	id INT PRIMARY KEY,
	stuName VARCHAR(20) NOT NULL,
	gender CHAR(1) CHECK(gender='男' OR gender='女')，
	seat INT UNIQUE,
	age INT DEFAULT 18,


};
/*
#添加表级约束
语法：
在各字段最下面
【constraint 约束名】约束类型（字段名）

*/
CREATE TABLE stuinfo{
	id BIGINT PRIMARY KEY,
	stuName VARCHAR(20) NOT NULL,
	gender CHAR(1) CHECK(gender='男' OR gender='女')，
	seat INT UNIQUE,
	age INT DEFAULT 18,
	CONSTRAINT pk PRIMARY KEY(id),#主键
	CONSTRAINT fk_stuinfl_major FOREIGN KEY(majorid) REFERENCES major(id)#外键

};



CREATE DATABASE IF NOT EXISTS tracks;
ALTER DATABASE tracks CHARACTER SET utf8;#改成utf8可以使用中文字符
USE tracks;
CREATE TABLE IF NOT EXISTS register_info(
	id BIGINT(11) PRIMARY KEY,#编号
	real_name VARCHAR(4) UNIQUE NOT NULL,#
	gender CHAR(1),
	address VARCHAR(20) NOT NULL
);
CREATE TABLE IF NOT EXISTS track(
	numb BIGINT(11)NOT NULL,
	re_time DATETIME NOT NULL,
	latitude DOUBLE NOT NULL,
	longitude DOUBLE NOT NULL,
	scale INT NOT NULL DEFAULT 5,
	CONSTRAINT fk_track_register FOREIGN KEY(numb) REFERENCES register_info(id)
);

ALTER TABLE register_info CONVERT TO CHARACTER SET utf8;#使用中文字符
ALTER TABLE track CONVERT TO CHARACTER SET utf8;#使用中文字符




SELECT * FROM track;
SELECT * FROM register_info;

DROP DATABASE tracks;
DROP TABLE register_info;
INSERT INTO register_info  VALUES (19852649073, '张三','男','北京');
INSERT INTO track(numb, re_time, latitude, longitude, scale)VALUES(19853649074,"1992-04-03 00:00:00",
                                                                      106.725241,26.594446,5);
(id, real_name, gender, address) 

`register_info``register_info`

# "numb=19853649073&time=1992-04-03 00:00:00&latitude=106.725241&longitude=26.594446
INSERT INTO book (id, bName, price, authorId, pubDate) VALUES ( 23, 'sfjksf',23.5, 123, NULL);

INSERT INTO int_tab (t1, t2) VALUES (12, 34);

INSERT INTO register_info(id, real_name, gender, address)VALUES(19854536783, "王三", "男", "上海");





SELECT * FROM int_tab;
SELECT VERSION();





SHOW VARIABLES LIKE '%max_allowed_packet%';


SET GLOBAL max_allowed_packet = 200 *1024*1024;

































