# 绘制框架图
import numpy as np
import cv2
from PIL import Image, ImageDraw, ImageFont

img = np.zeros((600, 800), dtype=np.uint8)
img += 255

'''
    矩形框
'''
# 硬件底层
cv2.rectangle(img, (710,480), (790,590), (0,0,0), 1)

# 运行时
cv2.rectangle(img, (710,280), (790,470), (0,0,0), 1)

# 应用层
cv2.rectangle(img, (710,10), (790,270), (0,0,0), 1)

# 硬件资源
cv2.rectangle(img, (10,480), (700,590), (0,0,0), 1)
# CPU
cv2.rectangle(img, (60,485), (370,585), (0,0,0), 1)
# 主存
cv2.rectangle(img, (65,490), (365,535), (0,0,0), 1)
# CPU1
cv2.rectangle(img, (65,540), (210,580), (0,0,0), 1)
# CPU2
cv2.rectangle(img, (220,540), (365,580), (0,0,0), 1)
# GPU
cv2.rectangle(img, (385,485), (695,585), (0,0,0), 1)
# 显存
cv2.rectangle(img, (390,490), (690,535), (0,0,0), 1)
# GPU1
cv2.rectangle(img, (390,540), (459,580), (0,0,0), 1)
# GPU2
cv2.rectangle(img, (467,540), (536,580), (0,0,0), 1)
# GPU3
cv2.rectangle(img, (544,540), (613,580), (0,0,0), 1)
# GPU4
cv2.rectangle(img, (621,540), (690,580), (0,0,0), 1)

# Linux OS
cv2.rectangle(img, (10,420), (700,470), (0,0,0), 1)
# 进程
cv2.rectangle(img, (60,425), (210,465), (0,0,0), 1)
# 共享内存
cv2.rectangle(img, (220,425), (370,465), (0,0,0), 1)
# GPU驱动
cv2.rectangle(img, (385,425), (695,465), (0,0,0), 1)

# 执行层
cv2.rectangle(img, (10,280), (700,410), (0,0,0), 1)
# 调度器
cv2.rectangle(img, (60,285), (130,405), (0,0,0), 1)
# 日志系统
cv2.rectangle(img, (140,285), (210,340), (0,0,0), 1)
# 计时API
cv2.rectangle(img, (140,350), (210,405), (0,0,0), 1)
# 共享内存API
cv2.rectangle(img, (220,285), (370,340), (0,0,0), 1)
# 共享内存管理
cv2.rectangle(img, (220,350), (370,405), (0,0,0), 1)
# PyTorch 缓存层
cv2.rectangle(img, (385,285), (695,320), (0,0,0), 1)
# 显存管理插件
cv2.rectangle(img, (385,328), (695,363), (0,0,0), 1)
# 共享显存API
cv2.rectangle(img, (385,371), (695,405), (0,0,0), 1)

# 应用开发
cv2.rectangle(img, (10,60), (350,270), (0,0,0), 1)
# MarkDown工具
cv2.rectangle(img, (15,90), (175,160), (0,0,0), 1)
# Python API
cv2.rectangle(img, (185,90), (345,160), (0,0,0), 1)
# Queue
cv2.rectangle(img, (15,170), (55,265), (0,0,0), 1)
# Function组件库
cv2.rectangle(img, (65,170), (345,265), (0,0,0), 1)
# C++ Function 模板
cv2.rectangle(img, (70,220), (205,260), (0,0,0), 1)
# Python Function 模板
cv2.rectangle(img, (210,220), (340,260), (0,0,0), 1)

# 应用部署
cv2.rectangle(img, (360,60), (700,270), (0,0,0), 1)
# 部署器
cv2.rectangle(img, (365,90), (695,200), (0,0,0), 1)
# 配置文件
cv2.rectangle(img, (630,95), (690,195), (0,0,0), 1)
# 资源分配算法
cv2.rectangle(img, (450,150), (620,195), (0,0,0), 1)
# 功能分配算法
cv2.rectangle(img, (450,95), (620,140), (0,0,0), 1)
# 评测器
cv2.rectangle(img, (365,210), (695,265), (0,0,0), 1)
# 可视化工具
cv2.rectangle(img, (560,215), (690,260), (0,0,0), 1)

# 应用开发标题
cv2.rectangle(img, (10,10), (350,50), (0,0,0), 1)

# 应用部署标题
cv2.rectangle(img, (360,10), (700,50), (0,0,0), 1)

'''
    文字
'''
cv2_im = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
pil_im = Image.fromarray(cv2_im)
draw = ImageDraw.Draw(pil_im)
font1 = ImageFont.truetype("sys.ttf", 20, encoding="utf-8")
font2 = ImageFont.truetype("lls.ttf", 20, encoding="utf-8")
font3 = ImageFont.truetype("lls.ttf", 15, encoding="utf-8")

# 硬件底层
draw.text((732, 513), "硬件", (0,0,0), font=font1)
draw.text((732, 533), "底层", (0,0,0), font=font1)

# 运行时
draw.text((742, 313), "运", (0,0,0), font=font1)
draw.text((742, 363), "行", (0,0,0), font=font1)
draw.text((742, 413), "时", (0,0,0), font=font1)

# 应用层
draw.text((742, 40), "应", (0,0,0), font=font1)
draw.text((742, 130), "用", (0,0,0), font=font1)
draw.text((742, 220), "层", (0,0,0), font=font1)

# 硬件资源
draw.text((12, 510), "硬 件", (0,0,0), font=font2)
draw.text((12, 536), "资 源", (0,0,0), font=font2)
# 主存
draw.text((165, 505), "主             存", (0,0,0), font=font2)
# CPU
draw.text((122, 550), "CPU", (0,0,0), font=font2)
draw.text((273, 550), "CPU", (0,0,0), font=font2)
# PCIe
draw.text((373, 500), "P", (0,0,0), font=font3)
draw.text((373, 518), "C", (0,0,0), font=font3)
draw.text((373, 536), "I", (0,0,0), font=font3)
draw.text((373, 552), "e", (0,0,0), font=font3)
# 显存
draw.text((485, 505), "显             存", (0,0,0), font=font2)
# GPU
draw.text((405, 550), "GPU", (0,0,0), font=font2)
draw.text((480, 550), "GPU", (0,0,0), font=font2)
draw.text((560, 550), "GPU", (0,0,0), font=font2)
draw.text((635, 550), "GPU", (0,0,0), font=font2)

# Linux OS
draw.text((14, 427), "Linux", (0,0,0), font=font3)
draw.text((14, 446), "  OS ", (0,0,0), font=font3)
# 进程
draw.text((110, 433), "进   程", (0,0,0), font=font2)
# 共享内存
draw.text((250, 433), "共 享 内 存", (0,0,0), font=font2)
# GPU驱动
draw.text((495, 433), "GPU 驱动", (0,0,0), font=font2)

# 执行层
draw.text((12, 320), "执 行", (0,0,0), font=font2)
draw.text((12, 350), "  层 ", (0,0,0), font=font2)
# 调度器
draw.text((85, 305), "调", (0,0,0), font=font2)
draw.text((85, 335), "度", (0,0,0), font=font2)
draw.text((85, 365), "器", (0,0,0), font=font2)
# 日志系统
draw.text((150, 295), "日 志", (0,0,0), font=font2)
draw.text((150, 315), "系 统", (0,0,0), font=font2)
# 计时API
draw.text((150, 355), "计 时", (0,0,0), font=font2)
draw.text((150, 375), " API", (0,0,0), font=font2)
# 共享内存API
draw.text((235, 305), "共享内存API", (0,0,0), font=font2)
# 共享内存管理
draw.text((235, 370), "共享内存管理", (0,0,0), font=font2)
# 共享显存API
draw.text((480, 292), "共享显存API", (0,0,0), font=font2)
# 显存管理插件
draw.text((475, 335), "显存管理插件", (0,0,0), font=font2)
# PyTorch 缓存层
draw.text((470, 380), "PyTorch缓存层", (0,0,0), font=font2)

# 应用开发
draw.text((12, 65), "应用开发", (0,0,0), font=font2)
# MarkDown工具
draw.text((23, 115), "MarkDown工具", (0,0,0), font=font2)
# Python API
draw.text((210, 115), "Python API", (0,0,0), font=font2)
# Queue
draw.text((25, 190), "队", (0,0,0), font=font2)
draw.text((25, 220), "列", (0,0,0), font=font2)
# Function组件库
draw.text((123, 185), "Function 组件库", (0,0,0), font=font2)
# C++ Function 模板
draw.text((90, 220), "C++ Function", (0,0,0), font=font3)
draw.text((90, 240), "        模板   ", (0,0,0), font=font3)
# Python Function 模板
draw.text((215, 220), "Python Function", (0,0,0), font=font3)
draw.text((215, 240), "           模板     ", (0,0,0), font=font3)

# 应用部署
draw.text((362, 65), "应用部署", (0,0,0), font=font2)
# 部署器
draw.text((385, 115), "部 署", (0,0,0), font=font2)
draw.text((385, 150), "  器 ", (0,0,0), font=font2)
# 配置文件
draw.text((640, 115), "配置", (0,0,0), font=font2)
draw.text((640, 155), "文件", (0,0,0), font=font2)
# 资源分配算法
draw.text((473, 163), "资源分配算法", (0,0,0), font=font2)
# 功能放置算法
draw.text((473, 110), "功能放置算法", (0,0,0), font=font2)
# 评测器
draw.text((428, 227), "评 测 器", (0,0,0), font=font2)
# 可视化工具
draw.text((578, 227), "可视化工具", (0,0,0), font=font2)

# 应用开发标题
draw.text((130, 20), "应  用  开  发", (0,0,0), font=font2)

# 应用部署标题
draw.text((470, 20), "应  用  部  署", (0,0,0), font=font2)

# 添加箭头顺序
draw.text((345, 100), "①", (0,0,0), font=font1)
draw.text((347, 230), "②", (0,0,0), font=font1)
draw.text((630, 280), "③", (0,0,0), font=font1)
draw.text((395, 195), "④", (0,0,0), font=font1)

img = cv2.cvtColor(np.array(pil_im), cv2.COLOR_BGR2RGB)

# 绘制箭头
# 应用开发到应用部署
img = cv2.arrowedLine(img, (335,127), (375,127), (0,0,0), 2, tipLength = 0.2)
# 应用部署到运行时  
img = cv2.arrowedLine(img, (377,185), (377,300), (0,0,0), 2, tipLength = 0.1)  
# 运行时到评测器
img = cv2.arrowedLine(img, (750,300), (550,250), (0,0,0), 2, tipLength = 0.05)  
# 评测器到部署器
img = cv2.arrowedLine(img, (390,230), (390,180), (0,0,0), 2, tipLength = 0.2)  

# 保存图片
cv2.imwrite("1.png", img)

