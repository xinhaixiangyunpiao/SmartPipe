# tasks:
    |  Task id  | Input | Output |            Functions             |                          Params                          |
    | --------- | ----- | ------ | -------------------------------- | -------------------------------------------------------- |
    |     0     | None  |    1   |   Image - Gen - genFromMemory    | 10,"/data/lx/SmartPipe/data_source/videos/road.webm",50  |
    |     1     |   0   |    2   |     Image - Resize - resize      |                         1920,1080                        |
    |     2     |   1   |  None  |  Model - Yolo - yolo_inference   |                           None                           |

# deploy:
    | Lcore id | Tasks id |
    | -------- | -------- |
    |    0     |    0     |
    |    1     |    1     |
    |    2     |    2     |

# dag:
    0 -> 1 -> 2