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
    longitude = request.args.get('longitude')
    latitude = request.args.get('latitude')
    print('longitude:' + longitude + ',' + 'latitude:' + latitude)
    with open('data/position.json', 'w', encoding='utf-8') as f:
        json.dump({'longitude':longitude, 'latitude':latitude},f)
    return jsonify(status='success')

@app.route('/downloadPosition')
def download_position():
    with open('data/position.json', 'r', encoding='utf-8') as f:
        position = json.load(f)
    return jsonify(position), 200, {"Content-Type":"application/json"}


@app.route('/uploadAwakening')
def upload_awakening():
    is_being_called = request.args.get('isBeingCalled')
    print('isBeingCalled:' + is_being_called)
    with open('data/beingcalled.json', 'w', encoding='utf-8') as f:
        json.dump({'isBeingCalled':is_being_called }, f)
    return jsonify(status='success')

@app.route('/downloadAwakening')
def download_awakening():
    with open('data/beingcalled.json', 'r', encoding='utf-8') as f:
        is_being_called = json.load(f)
    return jsonify(is_being_called), 200, {"Content-Type":"application/json"}

if __name__ == '__main__':
    # **这里得“0.0.0.0”代表任何ip都可访问，并非写成你的ip地址**端口是5000，你的安全组配置5000端口一定要打开
    app.run(host='0.0.0.0', port=conf.get_port())
