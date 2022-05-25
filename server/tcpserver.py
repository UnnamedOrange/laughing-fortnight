import socket
from socket import error as SocketError
import errno
import threading
from time import sleep
import requests


HOST = '172.24.132.39'               # 允许任意host接入
PORT = 12345
BUFSIZ = 1024
ADDR = (HOST, PORT)

CLOUDBASE = 'https://django-qix2-1901017-1311749828.ap-shanghai.run.tcloudbase.com/api/'

tcpSerSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpSerSock.setsockopt(
    socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # 强制使用端口，防止被占用的情况
tcpSerSock.bind(ADDR)   # 绑定地址
tcpSerSock.listen(1)    # 同时只允许一个TCP连接


while True:
    try:
        print('waiting for connection...')
        # 接受客户端请求之前保持阻塞，连接后获取客户端socket及其地址
        tcpCliSock, addr = tcpSerSock.accept()
        tcpCliSock.settimeout(0.5)

        # 打印请求此次服务的客户端的地址
        print('...connection from: {}'.format(addr))
        while True:
            # 通过客户socket获取客户端信息(bytes类型)，并解码为字符串类型
            try:
                istimeout = False
                data = tcpCliSock.recv(BUFSIZ).decode('utf8')
            except socket.timeout:
                data = ''
                istimeout = True
            except KeyboardInterrupt:
                # 只是中断本次连接
                # TODO 测试这个异常处理
                tcpCliSock.close() 
                raise KeyboardInterrupt
            except Exception as e:
                print(e)
            finally:
                if istimeout == False and not data:
                    break # 非超时的情况且没有收到数据，说明服务端主动终止或客户端已经退出
                print('recv once:', data, 'data length:', len(data))

    except SocketError as e:
        if e.errno != errno.ECONNRESET:
            raise
        print('reset error')
        tcpCliSock.close()
    except KeyboardInterrupt:
        try:
            tcpCliSock.close()
            print("closing")
        except NameError:
            pass
        break
