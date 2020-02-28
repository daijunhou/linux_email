base64 转换码：https://www.sojson.com/base64.html
MTg1NTAyMDAzMDQ=  //用户名的base64编码 用户名字：18550200304
MXFhejJ3c3g=		//授权码的base64编码 授权码:1qaz2wsx
——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
telnect 的方式发送邮件：
1.连接服务器
　　在终端上输入：telnet smtp.163.com 25 回车，然后就连接了服务器的25端口，成功会输出 220 163.com Anti-spam GT for Coremail System (163com[20141201])
2.表明身份，需要输入：helo 163.com 回车，顺利的话会出现 250 OK

3.登录认证，输入：auth login 回车，成功返回 334 dXNlcm5hbWU6

4.登录，用户名的base64编码。返回 334 UGFzc3dvcmQ6

5.输入授权码的base64编码，返回 235 Authentication successful

6.邮件正文
　　　　发件邮箱：mail from:<18550200304@163.com> 返回 250 Mail OK
　　　　收件邮箱：rcpt to:<1102214616@qq.com> 返回 250 Mail OK
　　　　输入：data 返回 354 End data with <CR><LF>.<CR><LF>
　　　　输入：subject:hello 这里需要注意，要两次回车
		输入：发送的邮箱
　　　　邮件正文：这里就随便写了
　　　　结束符号：这里要输入 . 表示邮件结束了，之后回车，发送成功会有250开头的返回
data
354 End data with <CR><LF>.<CR><LF>
subject:hello 
from:18550200304@163.com

this is contect 

.

实例：
houdaijun@houdaijun-VirtualBox:~/project/linux_email$ telnet smtp.163.com 25 
Trying 123.126.97.1...
Connected to smtp.163.com.
Escape character is '^]'.
220 163.com Anti-spam GT for Coremail System (163com[20141201])
helo 163.com
250 OK
auth login
334 dXNlcm5hbWU6
MTg1NTAyMDAzMDQ=
334 UGFzc3dvcmQ6
MXFhejJ3c3g=
235 Authentication successful
mail from:<18550200304@163.com>
250 Mail OK
rcpt to:<1102214616@qq.com>
250 Mail OK
data
354 End data with <CR><LF>.<CR><LF>
subject:monitor  
from:18550200304

this is a monitor mail 

.
250 Mail OK queued as smtp1,GdxpCgCn15dixldeSQdrBw--.54S2 1582810834

421 closing transmission channel
Connection closed by foreign host.
——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
email 的方式发送邮件：

sudo apt-get install heirloom-mailx
sudo apt-get install mailutils
sudo vim /etc/s-nail.rc
set from=18550200304@163.com
set smtp=smtps://smtp.163.com
set smtp-auth-user=18550200304@163.com
set smtp-auth-password=1qaz2wsx
set smtp-auth=login

set from 使用的邮箱 
set smtp-auth-user 使用的邮箱 
set smtp-auth-password 不是邮箱密码，是客户端授权密码
发送方式mail -s "test" jack.huang@ecovacs.com < 1.c
——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
　　•554 DT:SPM 发送的邮件内容包含了未被许可的信息，或被系统识别为垃圾邮件。请检查是否有用户发送病毒或者垃圾邮件；
　　•554 DT:SUM 信封发件人和信头发件人不匹配；
　　•554 IP is rejected, smtp auth error limit exceed 该IP验证失败次数过多，被临时禁止连接。请检查验证信息设置；
　　•554 HL:IHU 发信IP因发送垃圾邮件或存在异常的连接行为，被暂时挂起。请检测发信IP在历史上的发信情况和发信程序是否存在异常；
　　•554 HL:IPB 该IP不在网易允许的发送地址列表里；
　　•554 MI:STC 发件人当天内累计邮件数量超过限制，当天不再接受该发件人的投信。请降低发信频率；
　　•554 MI:SPB 此用户不在网易允许的发信用户列表里；
　　•554 IP in blacklist 该IP不在网易允许的发送地址列表里。


https://blog.csdn.net/huochen1994/article/details/51282093?utm_source=blogxgwz2
https://www.cnblogs.com/myccloves/p/9420584.html