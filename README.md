# Robot Project

Repository of the robot project from the OS course at Eurecom. 

The main objective of this project is to score points during a game by identifying objects and sending balls into the opponent's area. 

Each time an object is identified, information about the object are communicated to the server. 

#### Cross compilation
Compilation made with ev3dev.
It is a Debian Linux-based operating system that runs on several LEGO® MINDSTORMS compatible platforms including the LEGO® MINDSTORMS EV3 

It can be downloaded from https://www.ev3dev.org/

Command to cross compile:
{% filename %}command-line{% endfilename %}
docker run --rm -it -h ev3 -v RobotProject/:/src -w /src ev3cc /bin/bash
