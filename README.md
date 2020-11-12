# Robot Project

Repository of the robot project from the OS course at Eurecom. 

The main objective of this project is to score points during a game by identifying objects and sending balls into the opponent's area. 

Each time an object is identified, information about the object are communicated to the server. 

#### Command for cross compilation
docker run --rm -it -h ev3 -v ~/Documents/OS_robot/robot-project/robot_code/:/src -w /src ev3cc /bin/bash
