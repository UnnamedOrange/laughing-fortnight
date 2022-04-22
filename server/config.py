import configparser

class Config:
    def __init__(self, config_file_path=".conf"):
        self.config = configparser.ConfigParser()
        self.config_file_path = config_file_path

    def get(self, section, item):
        try:
            return self.config[section][item]
        except KeyError:
            self.config.read(self.config_file_path)
            return self.config[section][item]

    def get_domain(self):
        self.get("Domain", "ServerDomain")

    def get_port(self):
        self.get("Default", "Port")
