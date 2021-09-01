FROM ros:foxy
SHELL ["/bin/bash", "-c"]
ENV DEBIAN_FRONTEND=noninteractive
ENV DEBCONF_NOWARNINGS=yes

RUN export http_proxy=http://192.168.10.89:1081
RUN export https_proxy=http://192.168.10.89:1081
RUN git config --global http.proxy socks5://192.168.10.89:1080
RUN git config --global https.proxy socks5://192.168.10.89:1080

RUN sudo apt-get update && sudo apt-get -y install python3-pip python3-rospkg python3-rosdep
RUN pip3 install -U xmlschema -i https://pypi.douban.com/simple/
RUN pip3 install -U yamale -i https://pypi.douban.com/simple/

WORKDIR /home/ubuntu/Desktop/scenario_simulator_ws/src/scenario_simulator
COPY . $WORKDIR

WORKDIR /home/ubuntu/Desktop/scenario_simulator_ws/
RUN mkdir -p /home/ubuntu/Desktop/scenario_simulator_ws/src/scenario_simulator/external
WORKDIR /home/ubuntu/Desktop/scenario_simulator_ws/src/scenario_simulator
RUN sh install_depends.sh foxy

ENV DEBIAN_FRONTEND=noninteractive
RUN sudo apt-get update && apt-get install -y keyboard-configuration

RUN echo 'Etc/UTC' > /etc/timezone && \
    apt-get update && \
    apt-get install -q -y --no-install-recommends tzdata && \
    rm -rf /var/lib/apt/lists/*

setup environment
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

RUN sudo apt-get update && sudo apt-get -y install curl gnupg2 lsb-release
RUN sudo curl -sSL http://deb.autocore.ai/repo/acpilot/key.gpg  -o /usr/share/keyrings/autocore-acpilot.gpg
RUN sudo apt-key add /usr/share/keyrings/autocore-acpilot.gpg
RUN echo "deb [arch=$(dpkg --print-architecture)] http://deb.autocore.ai/repo/acpilot $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/acpilot.list > /dev/null

RUN sudo apt-get update && sudo apt-get install -y ros-foxy-suburbs-pnc
RUN sudo apt-get update && sudo apt-get install -y ros-foxy-system-launch ros-foxy-shape-estimation ros-foxy-awapi-awiv-adapter

WORKDIR /home/ubuntu/Desktop/scenario_simulator_ws/src
RUN source /opt/ros/foxy/setup.bash && rosdep install -iry --from-paths . --rosdistro foxy
WORKDIR /home/ubuntu/Desktop/scenario_simulator_ws
RUN source /opt/ros/foxy/setup.bash && colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release
COPY ./docker-entrypoint.sh /
RUN chmod a+x /docker-entrypoint.sh
RUN sudo apt-get update && apt-get install nano

# FROM ssv:first

RUN mkdir -p  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/test_scenario
RUN mkdir -p  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/map
COPY work_flow/work_flow.yaml  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/
COPY work_flow/map/*  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/map/
COPY work_flow/test_scenario/*  /home/ubuntu/Desktop/scenario_simulator_ws/work_flow/test_scenario/
COPY ./startup.sh  /home/ubuntu/Desktop/scenario_simulator_ws/
RUN chmod a+x /home/ubuntu/Desktop/scenario_simulator_ws/startup.sh

ENTRYPOINT ["/docker-entrypoint.sh"]
