# tasks:
    |  Task id  | Input | Output |         Functions           |                          Params                          |
    | --------- | ----- | ------ | --------------------------- | -------------------------------------------------------- |
    |     0     | None  |    1   | Image - Gen - genFromMemory | 25,"/data/lx/SmartPipe/data_source/videos/road.webm",900 |
    |     1     |   0   |    2   |   Image - Resize - resize   |                         1920,1080                        |
    |     2     |   1   |    3   |   Image - Resize - resize   |                          960,540                         |
    |     3     |   2   |  None  |   Image - Resize - resize   |                         3840,2160                        |

# dag:
    0 -> 1 -> 2 -> 3


