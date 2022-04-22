import sys
import unittest
import logging
import requests
import json

import config
conf = config.Config()

class TestServer(unittest.TestCase):
    def test_position(self):
        """
        @Description:
        测试位置信息的上传下载是否正常
        """

        log = logging.getLogger('TestServer.test_position')
        log.setLevel(logging.DEBUG)

        position = {'longitude': '123', 'latitude': '456'}
        r = requests.get(f"{conf.get_domain()}:{conf.get_port()}/uploadPosition", params=position)
        self.assertEqual(r.status_code, 200)

        r = requests.get(f"{conf.get_domain()}:{conf.get_port()}/downloadPosition")
        self.assertEqual(r.status_code, 200)

        received_position = json.loads(r.text)
        self.assertEqual(received_position, position)
    
    def test_caller(self):
        """
        @Description:
        测试呼叫功能相关状态变量
        """
        log = logging.getLogger('TestServer.test_caller')
        log.setLevel(logging.DEBUG)
        is_being_called = {'isBeingCalled': 'true'}
        r = requests.get(f"{conf.get_domain()}:{conf.get_port()}/uploadAwakening", params=is_being_called)
        self.assertEqual(r.status_code, 200)

        r = requests.get(f"{conf.get_domain()}:{conf.get_port()}/downloadAwakening")
        self.assertEqual(r.status_code, 200)

        received_position = json.loads(r.text)
        self.assertEqual(received_position, is_being_called)
        log.debug('test')


if __name__ == '__main__':
    logging.basicConfig( stream=sys.stderr )
    unittest.main()