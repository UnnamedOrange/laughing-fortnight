import configparser

class Config:
    def __init__(self, config_file_path=".conf"):
        self.config = configparser.ConfigParser()
        self.config_file_path = config_file_path # 配置文件保存位置

    def get(self, section, item):
        try:
            return self.config[section][item]
        except KeyError: # 缺页中断
            self.config.read(self.config_file_path)
            return self.config[section][item]

    def get_host(self): # 获取本机IP，用于server端
        return self.get("Host", "ServerHost")

    def get_port(self): # 获取端口
        return self.get("DEFAULT", "Port")

    def get_domain(self): # 获取域名，用于client端测试时使用
        return self.get("Host", "ServerDomain")