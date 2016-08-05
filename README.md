# Moses
Msh Object Set Exodus Saver

Finite Elements: Infinite Possibilities™

(Note: possibilities not actually infinite but, let's just say there are at least 2^8^2^32, and at most ~ 2^(10^7470) possibilities based on your compiler, a number so high it's basically infinity.)


Project Progress:

Mesh importing: Working (linear time)

Nodeset Detection: Working (linear time)

Sideset Detection: Works with first order 2D, tetrahedrons, and second order quads (n^2 time). Will add support for other 3d and second order shapes before the alpha release.

Exodus Files: Working. TODO: add 2d node support.


The only open source way to convert GMSH to Exodus nodesets et. all. Designed for programs like Seacas and Goma.

For more information on this and my other projects, please see the website.


### Building Moses

1) Clone this git repository.

2) Change into the dep repository.

`./buildDependancies.sh` (-j n to specify number of make jobs)

`mkdir build`

`cmake ..`

`make`

### License information
Moses, like all my other one man projects, is licensed under the GPLv3. It basically means you can reuse this code in your open source projects (or personal ones) as long as you keep it open source. If you want to use this code but not distribute it (editing it for your own benefit), the GPL allows you to do so. Fork my projects and let's make the best Exodus generator around.

### What does the name stand for?
Moses is the Msh Object Set Exodus Saver. It saves Gmsh .msh files as exodus files while extracting object sets (nodesets and sidesets).

### What are exodus files?
These are files used by SEACAS and other Sandia Laboritories code which was recently made open source. For more information on these files, [please look at their manuals](http://gsjaardema.github.io/seacas/). While billions of tax dollars in programs which use meshes were released to the public for free, no good mesh generators (cubit, for example) came alongside these. This project fills that gap by allowing you to generate meshes in gmsh and export them to exodus.

### How do I specify sidesets and nodesets?
I will create a guide on doing this before the final release. Effectively add lines, nodes, and eventually surfaces to tags and these will automatically detect side and nodesets.

### What is your progress?

My progress is as follows

Initial Build (100%):

[##################################################]

Alpha (40%):

[#################---------------------------------]

Version 1 (0%):

[--------------------------------------------------]
