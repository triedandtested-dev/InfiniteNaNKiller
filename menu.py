nodesmenu = nuke.menu("Nodes")

slyfoxmenu = nodesmenu.addMenu("SlyfoxFX","slyfoxfx.png")
slyfoxmenu.addCommand("InfiniteNaNKiller", "nuke.createNode(\"sf_InfiniteNaNKiller\")", icon="sf_killernan.png");
