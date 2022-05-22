import socket
from socket import error as SocketError
import errno


HOST = '172.24.132.39'               # 允许任意host接入
PORT = 12345
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpSerSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpSerSock.bind(ADDR)   # 绑定地址
tcpSerSock.listen(1)    # 同时只允许一个TCP连接


while True:
    try:
        print('waiting for connection...')
        # 接受客户端请求之前保持阻塞，连接后获取客户端socket及其地址
        tcpCliSock, addr = tcpSerSock.accept()
        # 打印请求此次服务的客户端的地址
        print('...connection from: {}'.format(addr))
        while True:
            # 通过客户socket获取客户端信息(bytes类型)，并解码为字符串类型
            data = tcpCliSock.recv(BUFSIZ).decode('utf8')
            if not data:
                break
            print(data)
            cmd = input("> ")
            # 处理字符串并重新编码为bytes类型，调用send()方法发送回客户端
            tcpCliSock.send(cmd.encode('utf8'))
    except SocketError as e:
        if e.errno != errno.ECONNRESET:
            raise
        print('reset error')
    except KeyboardInterrupt:
        print("closing")
        tcpCliSock.close()
        break
