kCornerNormal = 0
kCornerNorthWest = 1
kCornerNorthEast = 2
kCornerSouthWest = 3
kCornerSouthEast = 4

myGrid = [[]] #2d array of cells

#Create a corner class
#It stores a x,y location, and cornerers to each of the corners around it
class Corner
    constructor: (x, y, cornerType) ->
        @x = x
        @y = y
        @cornerType = cornerType
        
    @North: null
    @NorthWest: null
    @NorthEast: null
    @East: null
    @West: null
    @South: null
    @SouthWest: null
    @SouthEast: null
    
    #I decided to add Node variables as well
    @fVal = 999999999999999999
    @gVal = 999999999999999999
    @hVal = 999999999999999999
    @Parent = null
    
    getSuccessors: () ->
        successors = []
        if @North
            successors.push @North
        if @NorthWest
            successors.push @NorthWest
        if @NorthEast
            successors.push @NorthEast
        if @West
            successors.push @West
        if @East
            successors.push @East
        if @SouthWest
            successors.push @SouthWest
        if @SouthEast
            successors.push @SouthEast
        if @South
            successors.push @South
        return successors
    
class Cell
    constructor: (isObstacle) ->
        @isObstacle = isObstacle
        @northWest = null
        @northEast = null
        @southWest = null
        @southEast = null
    
class Obstacle
    constructor: (x, y) ->
        @gridX = x
        @gridY = y
    
#Create a Grid Class
#The class should hold all the Corners of the grid, given an input of size
class Grid
    constructor: (width, height) ->
        @Width = width
        @Height = height
        @Corners = []
        
    createEmptyGrid: () ->
        for row in [0...@Width]
            Row = []
            for col in [0...@Height]
                Row.push new Corner(row, col)
            @Corners.push Row
        console.log "Done creating Corners array."
        #Now link the Corners together
        for corners in @Corners
            for corner in corners
                if corner.y > 0
                    corner.North = @Corners[corner.x][corner.y-1]
                    if corner.x > 0
                        corner.NorthWest = @Corners[corner.x-1][corner.y-1]
                    if corner.x < @Width - 1
                        corner.NorthEast = @Corners[corner.x+1][corner.y-1]
                if corner.y < @Height - 1
                    corner.South = @Corners[corner.x][corner.y+1]
                    if corner.x > 0
                        corner.SouthWest = @Corners[corner.x-1][corner.y+1]
                    if corner.x < @Width - 1
                        corner.SouthEast = @Corners[corner.x+1][corner.y+1]
                if corner.x > 0
                    corner.West = @Corners[corner.x-1][corner.y]
                if corner.x < @Width - 1
                    corner.East = @Corners[corner.x+1][corner.y]
        console.log "Done linking corners together."
    
    createGrid: (Obstacles) ->
        console.log("Beginning to create grid with obstacles")
        #First create Empty corners array
        for row in [0...@Width]
            Row = []
            for col in [0...@Height]
                Row.push new corner(row, col)
            @Corners.push Row
        console.log "Done creating corners array. Now linking the corners."
        #First, link them all together in the normal way
        for corners in @Corners
            for corner in corners
                if corner.y > 0
                    corner.North = @Corners[corner.x][corner.y-1]
                    if corner.x > 0
                        corner.NorthWest = @Corners[corner.x-1][corner.y-1]
                    if corner.x < @Width - 1
                        corner.NorthEast = @Corners[corner.x+1][corner.y-1]
                if corner.y < @Height - 1
                    corner.South = @Corners[corner.x][corner.y+1]
                    if corner.x > 0
                        corner.SouthWest = @Corners[corner.x-1][corner.y+1]
                    if corner.x < @Width - 1
                        corner.SouthEast = @Corners[corner.x+1][corner.y+1]
                if corner.x > 0
                    corner.West = @Corners[corner.x-1][corner.y]
                if corner.x < @Width - 1
                    corner.East = @Corners[corner.x+1][corner.y]
        for obstacle in Obstacles
            console.log(obstacle.x, ",", obstacle.y)
            if @Corners[obstacle.x][obstacle.y]
                temp = @Corners[obstacle.x][obstacle.y].SouthEast
                if temp
                    @Corners[obstacle.x][obstacle.y].SouthEast = null
                    temp.NorthWest = null
                if @Corners[obstacle.x][obstacle.y].NorthEast == null
                    temp = @Corners[obstacle.x][obstacle.y].East
                    @Corners[obstacle.x][obstacle.y].East = null
                    temp.West = null
                if obstacle.y > 0
                    console.log "Then we can change the North"

                    
##MATH Functions##
#Square Fxn ya know, like x^2
square = (x) -> x * x

#Absolute value
Absolute = (a) ->
    if a < 0
        a = -a
    return a

##Heuristic Functions##
#Euclidean Distance for the first Heuristic. Pretty basic
EuclideanDistance = (startcorner, goalcorner) ->
    return Math.sqrt((square (startcorner.x - goalcorner.x)) + (square (startcorner.y - goalcorner.y)))

#Manhattan Distance
ManhattanDistance = (startCorner, goalCorner) ->
    return Absolute(goalCorner.y - startCorner.y) + Absolute(goalCorner.x - startCorner.x)

#Octile Distance
OctileDistance = (startCorner, goalCorner) ->
    dx = Absolute(startCorner.x - goalCorner.x)
    dy = Absolute(startCorner.y - goalCorner.y)
    return dx + dy + (1-Math.sqrt(2)) * (Math.min(dx, dy))

#CornerAtDiagonalIntersection method
CornerAtDiagonalIntersection = (x, y) ->
    #How do i do this fxn?
    return false

#IsTraversable method
IsTraversable = (x, y) ->
    if myGrid[y][x].isObstacle
        return false
    return true

#LineOfSight check method
LineOfSight = (startCorner, goalCorner) ->
    x1 = startCorner.x
    x2 = goalCorner.x
    y1 = startCorner.y
    y2 = goalCorner.y
    
    dy = y2 - y1
    dx = x2 - x1
    
    #Disallow movements between obstacles
    if dx > 0
        if startCorner.cornerType == kCornerNorthWest or startCorner.cornerType == kCornerSouthWest or goalCorner.cornerType == kCornerNorthEast or goalCorner.cornerType == kCornerSouthEast
            return false
        if dx < 0
            if startCorner.cornerType == kCornerNorthEast or startCorner.cornerType == kCornerSouthEast or goalCorner.cornerType == kCornerNorthWest or goalCorner.cornerType == kCornerSouthWest
                return false
        if dy > 0
            if startCorner.cornerType == kCornerNorthWest or startCorner.cornerType == kCornerNorthEast or goalCorner.cornerType == kCornerSouthWest or goalCorner.cornerType == kCornerSouthEast
                return false;
        if dy < 0
            if startCorner.cornerType == kCornerSouthWest or startCorner.cornerType == kCornerSouthEast or goalCorner.cornerType == kCornerNorthWest or goalCorner.cornerType == kCornerNorthEast
                return false;
            
    f = 0
    sy = 0
    sx = 0
    x_offset = 0
    y_offset = 0
    
    if dy < 0
        dy = -dy
        sy = -1
        y_offset = 0
    else 
        sy = 1
        y_offset = 1
    if dx < 0
        dx = -dx
        sx = -1
        x_offset = 0
    else
        sx = 1
        x_offset = 1
        
    if dx >= dy #Move along the x axis and increment/decrement y when f >= dx.
        while x1 != x2
            f = f + dy
            if f >= dx #We are changing rows, we might need to check two cells this iteration.
                if !IsTraversable(x1 + x_offset, y1 + y_offset) #TODO add this method
                    return false
                y1 = y1 + sy
                f = f - dx

            if f != 0 #If f == 0, then we are crossing the row at a corner point and we don't need to check both cells.
                if !IsTraversable(x1 + x_offset, y1 + y_offset)
                    return false

            if dy == 0 #If we are moving along a horizontal line, either the north or the south cell should be unblocked.
                if !IsTraversable(x1 + x_offset, y1) and !IsTraversable(x1 + x_offset, y1 + 1)
                    return false

            x1 += sx

            if f == 0 and x1 != x2 and CornerAtDiagonalIntersection(x1,y1)
                 return false
    else #if (dx < dy). Move along the y axis and increment/decrement x when f >= dy.
        while y1 != y2
            f = f + dx
            if f >= dy
                if !IsTraversable(x1 + x_offset, y1 + y_offset)
                    return false
                x1 = x1 + sx
                f = f - dy
            if f != 0
                if !IsTraversable(x1 + x_offset, y1 + y_offset)
                    return false
            if dx == 0
                if !IsTraversable(x1, y1 + y_offset) and !IsTraversable(x1 + 1, y1 + y_offset)
                    return false
            y1 += sy
            if f == 0 and y1 != y2 and CornerAtDiagonalIntersection(x1,y1)
                 return false;
    return true
        
class AStarSearch
    constructor: (heuristic) ->
        @Heuristic = heuristic
        @OpenList = []
        @ClosedList = []
        
    smoothPath: (path) ->
        smoothedPath = []
        if !path
            return path
        #First push the first index of path
        smoothedPath.push(path[0])
        for corner in path
            if !(LineOfSight(smoothedPath[smoothedPath.length - 1], corner))
                smoothedPath.push corner
        smoothedPath.push path[path.length - 1]
        return smoothedPath
        
    addToClosed: (node) ->
        @ClosedList.push node
        #Now remove this corner from OpenList, and add all others to OpenList
        indexToRemove = @OpenList.indexOf node
        #Index is -1 for the first index, because nothing should be in OpenList
        if !(indexToRemove < 0)
            #Remove!
            @OpenList.splice indexToRemove, 1
        #Now add successors to Open List
        for corner in node.getSuccessors()
            #If node is in ClosedList, don't do anything
            inClosed = false
            for inListNode in @ClosedList
                if inListNode.x is corner.x
                    if inListNode.y is corner.y
                        inClosed = true
                        break
            if inClosed
                continue
            indexOfNode = -1
            num = 0
            for inListNode in @OpenList
                if inListNode.x is corner.x
                    if inListNode.y is corner.y
                        #Save the index and break out!
                        indexOfNode = num
                        break
                num += 1
            if indexOfNode < 0
                #Not in ClosedList or OpenList. So we can add it!
                corner.gVal = node.gVal + 1
                if @Heuristic == "EuclideanDistance"
                    corner.hVal = EuclideanDistance(corner, @Goalcorner)
                else if @Heuristic == "OctileDistance"
                    corner.hVal = OctileDistance(corner, @Goalcorner)
                else
                    corner.hVal = ManhattanDistance(corner, @Goalcorner)
                corner.fVal = corner.gVal + corner.hVal
                corner.Parent = node
                @OpenList.push corner
            else
                #So the node is already in the OpenList. Well, does ours have a better fVal?
                ourFval = node.gVal + 1 + EuclideanDistance(corner, @Goalcorner)
                if ourFval < @OpenList[indexOfNode]
                    #Yes, we should add!
                    #First remove the previous
                    @OpenList.splice indexOfNode, 1
                    #And then we can add
                    corner.gVal = node.gVal + 1
                    if @Heuristic == "EuclideanDistance"
                        corner.hVal = EuclideanDistance(corner, @Goalcorner)
                    else if @Heuristic == "OctileDistance"
                        corner.hVal = OctileDistance(corner, @Goalcorner)
                    else
                        corner.hVal = ManhattanDistance(corner, @Goalcorner)
                    corner.fVal = corner.gVal + corner.hVal
                    corner.Parent = node
                    @OpenList.push corner
        
    search: (startcorner, goalcorner) ->
        @Goalcorner = goalcorner
        path = []
        if startcorner is goalcorner
            console.log "Start is Goal"
            return path
        #Make startcorner into a node
        startcorner.gVal = 0
        if @Heuristic == "EuclideanDistance"
            startcorner.hVal = EuclideanDistance(startcorner, goalcorner)
            startcorner.fVal = EuclideanDistance(startcorner, goalcorner)
        else if @Heuristic == "OctileDistance"
            startcorner.hVal = OctileDistance(startcorner, goalcorner)
            startcorner.fVal = OctileDistance(startcorner, goalcorner)
        else
            startcorner.hVal = ManhattanDistance(startcorner, goalcorner)
            startcorner.fVal = ManhattanDistance(startcorner, goalcorner)
            
        this.addToClosed startcorner
        
        reachedGoal = false
        
        while (@OpenList.length)
            #Get min from the OpenList and add it to the ClosedList
            minFval = 9999999999
            currNode = null
            for node in @OpenList
                if node.fVal < minFval
                    minFval = node.fVal
                    currNode = node
            if currNode
                this.addToClosed currNode
                #If this is the goal, stop
                if currNode is goalcorner
                    console.log "Reached goal!"
                    reachedGoal = true
                    break
            else
                console.log "Open list empty"
                break
        #Now Fill in Path
        #Basically as you are searching, set the parent. Then load the Goal corner. If it costed less than infinity, then a path was found. Basically keep going back until you have reached parent.
        if reachedGoal
            curr = goalcorner
            while curr != startcorner
                path.push curr
                curr = curr.Parent
            path.push startcorner
        path = path.reverse()
        #Now that we have the path, do los check for every points
        path = this.smoothPath path
        return path
            
#Runner Code
#TODO add a timer

class ThetaStarSearch
    constructor: (heuristic) ->
        @Heuristic = heuristic
        @OpenList = []
        @ClosedList = []
        
    smoothPath: (path) ->
        smoothedPath = []
        if !path
            return path
        #First push the first index of path
        smoothedPath.push(path[0])
        for corner in path
            if !(LineOfSight(smoothedPath[smoothedPath.length - 1], corner))
                smoothedPath.push corner
        smoothedPath.push path[path.length - 1]
        return smoothedPath
        
    addToClosed: (node) ->
        @ClosedList.push node
        #Now remove this corner from OpenList, and add all others to OpenList
        indexToRemove = @OpenList.indexOf node
        #Index is -1 for the first index, because nothing should be in OpenList
        if !(indexToRemove < 0)
            #Remove!
            @OpenList.splice indexToRemove, 1
        #Now add successors to Open List
        for corner in node.getSuccessors()
            #If node is in ClosedList, don't do anything
            inClosed = false
            for inListNode in @ClosedList
                if inListNode.x is corner.x
                    if inListNode.y is corner.y
                        inClosed = true
                        break
            if inClosed
                continue
            indexOfNode = -1
            num = 0
            for inListNode in @OpenList
                if inListNode.x is corner.x
                    if inListNode.y is corner.y
                        #Save the index and break out!
                        indexOfNode = num
                        break
                num += 1
            if indexOfNode < 0
                #Not in ClosedList or OpenList. So we can add it!
                corner.gVal = node.gVal + 1
                if node.parent
                    if LineOfSight(node.parent, corner)
                        corner.parent = node.parent
                        corner.gVal = corner.parent.gVal + EuclideanDistance(corner.parent, corner)
                if @Heuristic == "EuclideanDistance"
                    corner.hVal = EuclideanDistance(corner, @Goalcorner)
                else if @Heuristic == "OctileDistance"
                    corner.hVal = OctileDistance(corner, @Goalcorner)
                else
                    corner.hVal = ManhattanDistance(corner, @Goalcorner)
                corner.fVal = corner.gVal + corner.hVal
                corner.Parent = node
                @OpenList.push corner
            else
                #So the node is already in the OpenList. Well, does ours have a better fVal?
                ourFval = node.gVal + 1 + EuclideanDistance(corner, @Goalcorner)
                if node.parent
                    if LineOfSight(node.parent, corner)
                        #corner.parent = node.parent
                        ourFval = node.parent.gVal + EuclideanDistance(node.parent, corner)
                if ourFval < @OpenList[indexOfNode]
                    #Yes, we should add!
                    #First remove the previous
                    @OpenList.splice indexOfNode, 1
                    #And then we can add
                    corner.gVal = node.gVal + 1
                    if node.parent
                        if LineOfSight(node.parent, corner)
                            corner.parent = node.parent
                            corner.gVal = corner.parent.gVal + EuclideanDistance(corner.parent, corner)
                    if @Heuristic == "EuclideanDistance"
                        corner.hVal = EuclideanDistance(corner, @Goalcorner)
                    else if @Heuristic == "OctileDistance"
                        corner.hVal = OctileDistance(corner, @Goalcorner)
                    else
                        corner.hVal = ManhattanDistance(corner, @Goalcorner)
                    corner.fVal = corner.gVal + corner.hVal
                    corner.Parent = node
                    @OpenList.push corner
        
    search: (startcorner, goalcorner) ->
        @Goalcorner = goalcorner
        path = []
        if startcorner is goalcorner
            console.log "Start is Goal"
            return path
        #Make startcorner into a node
        startcorner.gVal = 0
        if @Heuristic == "EuclideanDistance"
            startcorner.hVal = EuclideanDistance(startcorner, goalcorner)
            startcorner.fVal = EuclideanDistance(startcorner, goalcorner)
        else if @Heuristic == "OctileDistance"
            startcorner.hVal = OctileDistance(startcorner, goalcorner)
            startcorner.fVal = OctileDistance(startcorner, goalcorner)
        else
            startcorner.hVal = ManhattanDistance(startcorner, goalcorner)
            startcorner.fVal = ManhattanDistance(startcorner, goalcorner)
            
        this.addToClosed startcorner
        
        reachedGoal = false
        
        while (@OpenList.length)
            #Get min from the OpenList and add it to the ClosedList
            minFval = 9999999999
            currNode = null
            for node in @OpenList
                if node.fVal < minFval
                    minFval = node.fVal
                    currNode = node
            if currNode
                this.addToClosed currNode
                #If this is the goal, stop
                if currNode is goalcorner
                    console.log "Reached goal!"
                    reachedGoal = true
                    break
            else
                console.log "Open list empty"
                break
        #Now Fill in Path
        #Basically as you are searching, set the parent. Then load the Goal corner. If it costed less than infinity, then a path was found. Basically keep going back until you have reached parent.
        if reachedGoal
            curr = goalcorner
            while curr != startcorner
                path.push curr
                curr = curr.Parent
            path.push startcorner
        path = path.reverse()
        #Now that we have the path, do los check for every points
        path = this.smoothPath path
        return path


grid = new Grid(30, 20)
grid.createEmptyGrid()
start = {x:0, y:0}
goal = {x:14, y:11}

#set myGrid

searchie = new ThetaStarSearch("OctileDistance")

path = searchie.search(grid.Corners[4][17], grid.Corners[17][4])

console.log "Pathie"
#Print Path
for corner in path
    console.log corner.x, corner.y