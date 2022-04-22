from flask import Flask, request, jsonify
import json
import config

app = Flask(__name__)
conf = config.Config()

# 检查经纬度信息的可行性
def check_titude(x):
    # @TODO: 使用正则表达式，后续和硬件端上传格式进行对接
    return True

@app.route('/uploadPosition')
def upload_position():
    """
    @Description:
    用于维护上传位置的接口`/uploadPosition`
    ---------
    @Returns:
    上传状态
    -------
    """
    
    longitude = request.args.get('longitude') # 获取request中的经度信息
    latitude = request.args.get('latitude') # 获取request中的纬度信息
    # print('longitude:' + longitude + ',' + 'latitude:' + latitude) # 打印经纬度信息，调试用
    with open('data/position.json', 'w', encoding='utf-8') as f:
        json.dump({'longitude':longitude, 'latitude':latitude},f) # 保存经纬度信息
    return jsonify(status='success') # 返回上传状态

@app.route('/downloadPosition')
def download_position():
    """
    @Description:
    用于维护下载位置的接口`/downloadPosition`
    ---------
    @Returns:
    当前服务器上保存的定位经纬度字符串
    -------
    """

    with open('data/position.json', 'r', encoding='utf-8') as f:
        position = json.load(f)
    return jsonify(position), 200, {"Content-Type":"application/json"}


@app.route('/uploadAwakening')
def upload_awakening():
    """
    @Description:
    用于维护上传触发唤醒的接口`/uploadAwakening`
    ---------
    @Returns:
    上传状态
    -------
    """
    
    is_being_called = request.args.get('isBeingCalled')
    print('isBeingCalled:' + is_being_called)
    with open('data/beingcalled.json', 'w', encoding='utf-8') as f:
        json.dump({'isBeingCalled':is_being_called }, f)
    return jsonify(status='success')

@app.route('/downloadAwakening')
def download_awakening():
    """
    @Description:
    用于维护下载触发唤醒的接口`/downloadAwakening`
    ---------
    @Returns:
    当前服务器上保存的触发唤醒状态，`1`为正在触发
    -------
    """

    with open('data/beingcalled.json', 'r', encoding='utf-8') as f:
        is_being_called = json.load(f)
    return jsonify(is_being_called), 200, {"Content-Type":"application/json"}

if __name__ == '__main__':
    # 这里得“0.0.0.0”代表任何ip都可访问，并非写成服务器的ip地址
    # 端口是5000，服务器安全组配置5000端口要打开
    app.run(host=conf.get_host(), port=conf.get_port())
