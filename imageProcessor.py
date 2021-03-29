import cv2
import numpy
import sys
from pathfinding.core.diagonal_movement import DiagonalMovement
from pathfinding.core.grid import Grid
from pathfinding.finder.a_star import AStarFinder

def doPathFind(pathToPng,startX, startY, endX, endY, createImg, pathToBackground):
    numpy.set_printoptions(threshold=sys.maxsize)
    decoded = cv2.imread(pathToPng, 0)
    tempList = []
    xi, yi = 0,0

    for x in decoded:
        for y in x:
            if y>0:
                decoded[int(xi)][int(yi)] = 1

            
            yi += 1
        
        yi = 0
        xi += 1

    matrix = decoded    

    grid = Grid(matrix=matrix)
    start = grid.node(startX, startY)
    end = grid.node(endX,endY)

    finder = AStarFinder(diagonal_movement=DiagonalMovement.always)
    path, runs = finder.find_path(start, end, grid)
    print('done!')
    if createImg:
        toEncode = cv2.imread(pathToBackground, 1)
        for coords in path:
            print(coords)
            toEncode[coords[1]][coords[0]] = (0, 255, 0)

        cv2.imshow('image', toEncode)
        cv2.waitKey(0)
    return(path)
