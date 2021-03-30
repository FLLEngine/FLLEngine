import cv2
import numpy
import sys
from pathfinding.core.diagonal_movement import DiagonalMovement
from pathfinding.core.grid import Grid
from pathfinding.finder.a_star import AStarFinder

def doPathFind(pathToPng,startX, startY, endX, endY, createImg, pathToBackground):
    numpy.set_printoptions(threshold=sys.maxsize)
    image = cv2.imread(pathToPng, 1)
    tempList = []
    imageShape = image.shape
    decoded = numpy.empty((imageShape[0],imageShape[1]))
    xi = 0
    for x in image:
        yi = 0
        for y in x:
            if y.tolist()==[0,0,0]:
                decoded[int(xi)][int(yi)] = 0
            elif y.tolist()==[255,255,255]:
                decoded[int(xi)][int(yi)] = 2
            elif y.tolist()==[0,255,0]:
                decoded[int(xi)][int(yi)] = 1
            elif y.tolist()==[255,0,0]:
                decoded[int(xi)][int(yi)] = 3

            yi += 1
        
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
            toEncode[coords[1]][coords[0]] = (0, 255, 0)

        cv2.imshow('image', toEncode)
        cv2.waitKey(0)
    return(path)
