from repo.yolov5.hubconf import yolov5x

# 载入模型
class yolo:
    # 构造
    def __init__(self):
        print("yolo is build.")

    # 载入模型
    def load_model(self):
        self.model = yolov5x()
        self.model = self.model.cuda()
        print("model load completed.")

    # 模型推理
    def inference(self, data):
        self.res = self.model(data)
        self.res.print()
        return self.res
    
    # Debug
    def debug(self):
        print("debug")