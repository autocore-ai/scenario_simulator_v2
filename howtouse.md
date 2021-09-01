# Scenario testing framework of Tier4 使用文档

## 一、打包测试工具镜像

1. clone 测试框架
   
```
git clone https://github.com/autocore-ai/scenario_simulator_v2.git
```

2. 执行
   
```
cd scenario_simulator_v2
sudo docker build -t scenario_simulator_v2 .
```

## 二、生成测试场景

使用Tier4提供的在线生成场景工具，生成被测试场景，并导出。

例如我们设定一个从起点到终点速度为1m/s直线行驶且无障碍物的场景，名称为 simple_test.yaml，存放在~/scenario_simulator_ws/scenarios/下

地址： https://scenario.tier4.jp/scenario_editor/restricted/
 
创建方法： https://tier4.github.io/scenario_simulator_v2/user_guide/scenario_editor/CreateSimpleScenario.html

编辑方法： https://tier4.github.io/scenario_simulator_v2/user_guide/scenario_editor/EditExistingScenario.html

## 三、将yaml文件的场景转换为openScenario文件

执行以下命令：

```
Python3 ~/scenario_simulator_ws/src/scenario_simulator_v2/openscenario/openscenario_utility/openscenario_utility/conversion.py --input "~/scenario_simulator_ws/scenarios/simple_test.yaml" --output "~/scenario_simulator_ws/scenarios/openscenarios/"
```

文件转换完成后，会在目录下看到simple_test.xosc

## 四、准备工作流文件

文件格式说明：

https://tier4.github.io/scenario_simulator_v2/user_guide/scenario_test_runner/HowToWriteWorkflowFile.html

比如工作流文件名称为：simple_example.yaml， 存放在：~/scenario_simulator_ws/scenarios/work_flow下
内容为：
```
Scenario:
  - {
      path: ~/scenario_simulator_ws/scenarios/openscenarios/simple_test.xosc,
      expect: success
    }
```
**注意： path的工作流文件不一定为OpenScenario的xosc文件，也可以是T4V2的yaml文件，即第三步不是必须。**

## 五、 场景地图

目前使用的地图是Tier4中的地图，如果要更换地图，记得将新的地图放到docker镜像中，实际可以拷贝到work_flow/map/下，对应生成的测试场景的yaml文件中地图路径也跟随改动。

例子：

1. 生成测试场景，文件名称为test.yaml

2. 编辑test.yaml，将osm的path替换成在docker中map的实际位置，注意为全路径

```
RoadNetwork:
    LogicFile:
      filepath: lanelet2_map.osm
```

替换成

```
RoadNetwork:
    LogicFile:
      filepath: /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/map/lanelet2_map.osm
```

## 六、测试work_flow打包进入docker镜像

1. 将work_flow目录copy到scenario_simulator_v2下
2. 将原镜像打一个tag，如: sudo docker tag scenario_simulator_v2:latest ssv:first
3. 注释掉Dockerfile文件中的内容
4. 将以下内容加入到dockerfile中
   
```
FROM ssv:first

RUN mkdir -p  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/test_scenario
RUN mkdir -p  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/map
COPY work_flow/work_flow.yaml  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/
COPY work_flow/map/*  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/map/
COPY work_flow/test_scenario/*  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/test_scenario/
COPY ./startup.sh  /home/ubuntu/Desktop/scenario_simulator_ws/

ENTRYPOINT ["/docker-entrypoint.sh"]
```

5. 打包镜像
   
```
sudo docker build -t scenario_simulator_v2 .
```

## 七、 执行case

```
sudo docker run -i scenario_simulator_v2 ./startup.sh
``` 

## 八、增加新case

1. 使用Tier4的场景工具，生成case yaml文件

2. 将yaml文件copy到work_flow/test_scenario目录下

3. 在work_flow.yaml文件中添加case

4. 重新打包镜像： sudo docker build -t scenario_simulator_v2 .

5. 执行case： sudo docker run -i scenario_simulator_v2 ./startup.sh

## 九、结果说明

1. Pass的case

case测试通过后，每个case的前边会有[PASSED]的字显示

```
+ ros2 run scenario_test_runner result_checker /tmp/scenario_test_runner/result.junit.xml
[PASSED] /tmp/scenario_test_runner/line_keep_normal_speed_case_1/line_keep_normal_speed_case_1.xosc
```

2. Failed的case

case测试通过后，每个case的前边会有[FAILED]的字显示

```
+ ros2 run scenario_test_runner result_checker /tmp/scenario_test_runner/result.junit.xml
[FAILED] /tmp/scenario_test_runner/line_keep_normal_speed_case_1/line_keep_normal_speed_case_1.xosc
```

3. 执行异常

若出现以下错误，则表示执行case中遇见问题，未生成测试结果，需要看具体错误原因

```
+ ros2 run scenario_test_runner result_checker /tmp/scenario_test_runner/result.junit.xml
Traceback (most recent call last):
  File "/home/ubuntu/Desktop/scenario_simulator_ws/install/scenario_test_runner/lib/scenario_test_runner/result_checker", line 11, in <module>
    load_entry_point('scenario-test-runner', 'console_scripts', 'result_checker')()
  File "/home/ubuntu/Desktop/scenario_simulator_ws/build/scenario_test_runner/scenario_test_runner/result_checker.py", line 80, in main
    checker.check(args.xml)
  File "/home/ubuntu/Desktop/scenario_simulator_ws/build/scenario_test_runner/scenario_test_runner/result_checker.py", line 44, in check
    suites = ET.parse(result).getroot()
  File "/usr/lib/python3.8/xml/etree/ElementTree.py", line 1202, in parse
    tree.parse(source, parser)
  File "/usr/lib/python3.8/xml/etree/ElementTree.py", line 584, in parse
    source = open(source, "rb")
FileNotFoundError: [Errno 2] No such file or directory: '/tmp/scenario_test_runner/result.junit.xml'

```
