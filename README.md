# Description

Bullet simulator.

## Build steps OS X

1. Install [XCode](https://developer.apple.com/xcode/)
1. Install [Brew](https://brew.sh)
1. Run *brew install glfw3*
1. Run *make*

## Problem

A list of segments (walls), represented by two points on a plane, is predefined.
Write BulletManager class with two public methods:

1. void Update (float time), where time – global update time in seconds. This method calculates bullet movement in given time, and in case of collision with the wall, removes the wall from the list and bullet continues movement with its trajectory reflected.
1. void Fire (float2 pos, float2 dir, float speed, float time, float life_time), where pos – starting point of the bullet on the plane (in meters), dir – direction, speed – bullet speed (m/s), time – when the bullet will be fired, life_time – time before bullet disappears. This method adds bullet to manager for further updates.
1. Update method will be called from main thread, but Fire method must be called from different threads.

### Requirements

You have to create visualization for this process. Render libraries can be used.

1. Language – C++.
1. Write clean and readable code.
1. Implement the system with the appropriate data structures. High level libraries such as STL or Boost can be used.
1. Optimize your code.
1. Comment the code to the degree that you feel is necessary.
1. Create 20+ bullets on each frame in parallel to the update (main) thread.
1. Create a test scenario in which you have 100+ bullets and 100+ walls. Add FPS counter overlay. FPS should not drop below 30.
1. Create a test scenario in which you have 1000+ bullets and 1000+ walls. Add FPS counter overlay. Try your best here with FPS.
