# CSCI441SMK

Contributors:
    Tyler Blount <trblount@mymail.mines.edu>
    
Clan:
    The Killer Rabbits of Caenbannog
    
Project:
    SMK
    
Description:
    Contains the program for the SMK project
    
Usage:
    ./SMK <track bezier file> <world bezier file>
    W, S move the user controlled vehicle forward and backward respectively
    A, D rotate the vehicle left and right
    Hold LMB and move the mouse to rotate the camera
    
Compiling:
    Use make to compile this program
    
Bugs:
    Vehicle pops up and down, and doesn't necessarily maintain orientation relative to the surface.
    We have been unable to determine why this occurs.

File Formats:
    Track file:
        <Number of Points>
        <x, y, z>
        .
        .
        .
        <x, y, z>
    
    Surface file:
        <Rows, Cols>
        <x y z> ... <x y z>
        .
        .
        .
        <x y z> ... <x y z>

Distribution:
    
    
Assignment time:
    Too much

Lab helpful?
    No

Fun?
    2 - This assignment either needs more time or more lectures on techniques to handle certain aspects.
