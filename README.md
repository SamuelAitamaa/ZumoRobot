# PSoC Creator project for Metropolia PSoC-Zumo adapter

This source code was used in the second period of the first year in Metropolia UAS. It consists of weekly labs and three main scripts for the final week: maze solving, line following and sumo wrestling for a Zumo Robot. The results were reported via MQTT. Everything was written in C. The source code can be found in ZumoRobot/ZumoBot.cydsn/main.c.

## Maze solving

In the maze solving the robot had to survive through a grid consisting obstacles and turns, following back lines. To accomplish this we used a classic maze solving algorithm, where the robot turned left every time it could. If it couldn't turn left due to obstacles, it would turn right. If that wasn't possible either, it would simply do a 180 degrees turn. The algorithm ended up as a success.

## Line Following

In this assignment the robot had to follow a 10-meter-long line with twists and turns with the best time possible. This one was all about speed and configuration of it's turns. For this we used specific variables to detect the offset of the sensors and the line, affecting the robot's speed directly. With this approach, we passed the task with a respectable speed.

## Sumo Wrestling

In the sumo wrestling competition the goal was to push other robots outside of a small ring. We made the robot to move towards the center and start spinning around it's own axel, detecting the enemy locations. Once it found something, it would simply rush towards it and try to push it out. When taking hits itself, it would make a correction move to get out of the trouble. The script was successful during practice matches, but during the final tournament we didn't find the success we were looking for.

## Contributors

* Samuel Aitamaa
* Jere Lampola
* Tuomas Rajala
