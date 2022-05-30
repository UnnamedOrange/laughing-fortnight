import socket
from socket import error as SocketError
import errno
import threading
from datetime import datetime, timedelta
import requests
from util.coord_trans import wgs84_to_gcj02


HOST = '172.24.132.39'               # 允许任意host接入
PORT = 12345
BUFSIZ = 1024
ADDR = (HOST, PORT)

CLOUDBASE = 'https://django-qix2-1901017-1311749828.ap-shanghai.run.tcloudbase.com/api/'

tcpSerSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpSerSock.setsockopt(
    socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # 强制使用端口，防止被占用的情况
tcpSerSock.bind(ADDR)   # 绑定地址
tcpSerSock.listen(0)

def set_keepalive_linux(sock, after_idle_sec=1, interval_sec=3, max_fails=5):
    """Set TCP keepalive on an open socket.

    It activates after 1 second (after_idle_sec) of idleness,
    then sends a keepalive ping once every 3 seconds (interval_sec),
    and closes the connection after 5 failed ping (max_fails), or 15 seconds
    """
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, after_idle_sec)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, interval_sec)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, max_fails)



def deg2dec(deg, min, sec):
    return deg + min/60 + sec/3600


def degmin2deg(degmin):
    deg_sep = degmin.find('.') - 2
    deg = int(degmin[:deg_sep])
    min = float(degmin[deg_sep:])
    return deg2dec(deg, min, 0)

buzz_state = 0
last_pulse_time = datetime.now() - timedelta(minutes=2)

while True:
    try:
        print('waiting for connection...')
        # 接受客户端请求之前保持阻塞，连接后获取客户端socket及其地址
        tcpCliSock, addr = tcpSerSock.accept()
        tcpCliSock.settimeout(0.5)

        set_keepalive_linux(tcpCliSock, 10)

        # 打印请求此次服务的客户端的地址
        print('...connection from: {}'.format(addr))
        while True:
            # 通过客户socket获取客户端信息(bytes类型)，并解码为字符串类型
            try:
                # 用于判别异常类型的flag
                istimeout = False

                # 维护心跳的计时
                if datetime.now() - last_pulse_time > timedelta(minutes=1):
                    last_pulse_time = datetime.now()
                    tcpCliSock.send('pulse'.encode('utf8'))

                # get云容器上保存的buzz信息
                if requests.get(CLOUDBASE + 'buzz').json().get('data') == 1 and buzz_state == 0:
                    tcpCliSock.send('buzz'.encode('utf8'))
                    buzz_state = 1
                    print('1', end='')
                else:
                    buzz_state = 0
                    print('0', end='')

                data = tcpCliSock.recv(BUFSIZ).decode('utf8') # recv是阻塞的，所以发get请求应该放在前面
                if data:
                    latitude, longitude = data[data.rfind('pos:')+4:-1].split(',')
                    longitude = degmin2deg(longitude)
                    latitude = degmin2deg(latitude)
                    longitude, latitude = wgs84_to_gcj02(longitude, latitude)
                    x = requests.post(CLOUDBASE + 'position',
                        json={"longitude": longitude, "latitude": latitude}) #注意这里是json=，否则会报500
                    print('debug:', longitude, latitude)
                    print(datetime.now())

            except socket.timeout:
                data = ''
                istimeout = True
            except KeyboardInterrupt:
                # 只是中断本次连接
                # TODO 测试这个异常处理
                tcpCliSock.close()
                raise KeyboardInterrupt
            finally:
                if istimeout == False and not data:
                    break  # 非超时的情况且没有收到数据，说明服务端主动终止或客户端已经退出
                # print('recv once:', data, 'data length:', len(data))

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
