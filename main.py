import pathfind

pathfind.doPathFind('/home/colin_nelson/Documents/robotics/matmasks/mainmask.png', 15, 22, 73, 192, createImg = True,smoothen=False ,pathToBackground = '/home/colin_nelson/Documents/robotics/matsmall.png')


pathfind.doPathFind('/home/colin_nelson/Documents/robotics/matmasks/mainmask.png', 15, 22, 73, 192,algo='dijkstra' ,smoothen=False ,createImg = True,pathToBackground = '/home/colin_nelson/Documents/robotics/matsmall.png')
