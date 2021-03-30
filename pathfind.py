import cv2
import numpy
import sys
from pathfinding.core.diagonal_movement import DiagonalMovement
from pathfinding.core.grid import Grid
from pathfinding.core.util import *
from pathfinding.finder import *

def doCreateImg(path, pathToBackground, name='image'):
    
    toEncode = cv2.imread(pathToBackground, 1)
    for coords in path:
        toEncode[coords[1]][coords[0]] = (0, 255, 0)

        cv2.imshow(name, toEncode)

def doPathFind(pathToPng,startX, startY, endX, endY, algo = 'a_star',smoothen=True, createImg=False, pathToBackground=''):
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

    if algo == 'a_star': 
        finder = a_star.AStarFinder(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == 'dijkstra':
        finder = dijkstra.DijkstraFinder(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == 'best_first':
        finder = best_first.BestFirst(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == 'bi_a_star':
        finder = bi_a_star.BiAStarFinder(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == 'breadth_first':
        finder = breadth_first.BreadthFirstFinder(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == 'ida_star':
        finder = ida_star.IDAStarFinder(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)
    elif algo == "msp":
        finder = msp.MinimumSpanningTree(diagonal_movement=DiagonalMovement.always)
        path, runs = finder.find_path(start, end, grid)

    if smoothen:
        path = smoothen_path(grid, path, use_raytrace=False)    

        
    print('done')
    if createImg:
        doCreateImg(path, pathToBackground, name=algo)
        cv2.waitKey(0)

    return(path)
