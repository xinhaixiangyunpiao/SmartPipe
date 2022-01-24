# tasks:
    |  Task id  | Type | Input | Output |         Functions           |                          Params                          |
    | --------- | ---- | ----- | ------ | --------------------------- | -------------------------------------------------------- |
    |     0     | CPU  | None  |    1   | Image - Gen - genFromMemory | 25,"/data/lx/SmartPipe/data_source/videos/road.webm",900 |
    |     1     | CPU  |   0   |    2   |    Image - Trans - trans    |                           None                           |
    |     2     | CPU  |   1   |    3   |    Image - Trans - trans    |                           None                           |
    |     3     | CPU  |   2   |    4   |    Image - Trans - trans    |                           None                           |
    |     4     | CPU  |   3   |    5   |    Image - Trans - trans    |                           None                           |
    |     5     | CPU  |   4   |    6   |    Image - Trans - trans    |                           None                           |
    |     6     | CPU  |   5   |    7   |    Image - Trans - trans    |                           None                           |
    |     7     | CPU  |   6   |    8   |    Image - Trans - trans    |                           None                           |
    |     8     | CPU  |   7   |    9   |    Image - Trans - trans    |                           None                           |
    |     9     | CPU  |   8   |   10   |    Image - Trans - trans    |                           None                           |
    |    10     | CPU  |   9   |  None  |    Image - Trans - trans    |                           None                           |

# dag:
    0 -> 1 -> 2 -> 3 ->4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10