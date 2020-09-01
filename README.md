# Bullet Manager

## Description

A list of segments (walls), represented by two points on a plane, is predefined.
There is a BulletManager class with two public methods:

1. Method that calculates bullet movement in given time, and in case of collision with the wall, removes the wall from the list and bullet continues movement with its trajectory reflected. Is called from main thread.

    ````C++
    void Update (float time)
    ````

    * time – global update time in seconds
1. Method that adds bullet to manager for further updates. Is called from different threads.

    ````C++
    void Fire(float2 pos, float2 dir, float speed, float time, float life_time);
    ````

    * pos – starting point of the bullet on the plane (in meters)
    * dir – direction
    * speed – bullet speed (m/s)
    * time – when the bullet will be fired
    * life_time – time before bullet disappears

Work of this class is visualized.

## Performance and stability requirements

1. 20+ bullets on each frame fired in parallel to the update (main) thread should not crash the app.
1. 100+ bullets and 100+ walls. FPS should not drop below 30.
1. 1000+ bullets and 1000+ walls. Should perform as well as possible.

## Run steps OS X

1. Install [XCode](https://developer.apple.com/xcode/)
1. Install [Brew](https://brew.sh)
1. Run *brew install glfw3*
1. Run *./run.sh*

## Tasks

Tuesday:

1. Draw predefined bullets and lines on screen.
1. Add bullets to the screen through bullet manager - single threaded.
1. Move bullets on the screen - single threaded.

Wednesday:

1. Implement collision detection and resolution - single threaded.

Thursday:

1. Display FPS counter.

Friday:

1. Optimize drawing and collision detection as much as possible. Try using broad and narrow collision phases.

Saturday:

1. Add multi-threaded bullet addition - check performance. Release 1.0.

Sunday - Tuesday:

1. If performance is not good enough try multi-threading physics calculations. Release 2.0
