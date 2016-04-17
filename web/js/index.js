var lines = [];
var canvases = [];

var AStarSearch, Absolute, Cell, Corner, CornerAtDiagonalIntersection, EuclideanDistance, Grid, IsTraversable, LineOfSight, ManhattanDistance, Obstacle, OctileDistance, ThetaStarSearch, corner, goal, grid, i, kCornerNormal, kCornerNorthEast, kCornerNorthWest, kCornerSouthEast, kCornerSouthWest, len, myGrid, path, searchie, square, start;

kCornerNormal = "normal";

kCornerNorthWest = "northWest";

kCornerNorthEast = "northEast";

kCornerSouthWest = "southWest";

kCornerSouthEast = "southEast";

function init(){
  canvases[0]=document.getElementById("mainCanvas");
        canvases[1]=document.getElementById("mainCanvas2");
        canvases[2]=document.getElementById("mainCanvas3");

       lines[0] =  canvases[0].getContext("2d");
        lines[1] =  canvases[1].getContext("2d");
         lines[2] =  canvases[2].getContext("2d");
        var unit = 30;

        lines[0] .lineWidth=1;
        lines[0] .beginPath();

        lines[1] .lineWidth=1;
        lines[1] .beginPath();

        lines[2] .lineWidth=1;
        lines[2] .beginPath();
};


angular.module('anyAngle', ['ngMaterial'])
  .controller('mainController', function($scope,$http) {

      init();


      //init grid
      $scope.size = {
        row: 20,
        column: 30
      };
      $scope.cell = [];
      $scope.cornersMap = [];
       $scope.corners = [];
      for(var i = 0 ; i < $scope.size.row; i ++){
          $scope.cell[i] = [];
        for (var j = 0; j < $scope.size.column ; j++){
          $scope.cell[i][j] = {};
          $scope.cell[i][j].isObstacle = false;
          $scope.cell[i][j].corners = [];
        }
      }
      for(var i = 0 ; i < $scope.size.row + 1; i ++){
          $scope.cornersMap[i] = [];
        for (var j = 0; j < $scope.size.column + 1; j++){
          $scope.cornersMap[i][j] = [];
        }
      }
      
      

      $scope.getNumber = function(num) {
          return new Array(num);
      };
      $scope.gridClicked = function(row, column){
        if(!(row == 19 && column == 0) && !(row == 0 && column == 29)){
            $scope.cell[row][column].isObstacle = !$scope.cell[row][column].isObstacle;
        }

      };

    

   
    
     square = function(x) {
    return x * x;
  };

  Absolute = function(a) {
    if (a < 0) {
      a = -a;
    }
    return a;
  };

  EuclideanDistance = function(startcorner, goalcorner) {
    return Math.sqrt((square(startcorner.x - goalcorner.x)) + (square(startcorner.y - goalcorner.y)));
  };

  ManhattanDistance = function(startCorner, goalCorner) {
    return Absolute(goalCorner.y - startCorner.y) + Absolute(goalCorner.x - startCorner.x);
  };

  OctileDistance = function(startCorner, goalCorner) {
    var dx, dy;
    dx = Absolute(startCorner.x - goalCorner.x);
    dy = Absolute(startCorner.y - goalCorner.y);
    return dx + dy + (1 - Math.sqrt(2)) * (Math.min(dx, dy));
  };

  CornerAtDiagonalIntersection = function(x, y) {
    return false;
  };

  IsTraversable = function(x, y) {
    if (myGrid[y][x].isObstacle) {
      return false;
    }
    return true;
  };
    
    LineOfSight = function(startCorner, goalCorner) {
    var dx, dy, f, sx, sy, x1, x2, x_offset, y1, y2, y_offset;
    x1 = startCorner.x;
    x2 = goalCorner.x;
    y1 = startCorner.y;
    y2 = goalCorner.y;
    dy = y2 - y1;
    dx = x2 - x1;
    if (dx > 0) {
      if (startCorner.cornerType === kCornerNorthWest || startCorner.cornerType === kCornerSouthWest || goalCorner.cornerType === kCornerNorthEast || goalCorner.cornerType === kCornerSouthEast) {
        return false;
      }
      if (dx < 0) {
        if (startCorner.cornerType === kCornerNorthEast || startCorner.cornerType === kCornerSouthEast || goalCorner.cornerType === kCornerNorthWest || goalCorner.cornerType === kCornerSouthWest) {
          return false;
        }
      }
      if (dy > 0) {
        if (startCorner.cornerType === kCornerNorthWest || startCorner.cornerType === kCornerNorthEast || goalCorner.cornerType === kCornerSouthWest || goalCorner.cornerType === kCornerSouthEast) {
          return false;
        }
      }
      if (dy < 0) {
        if (startCorner.cornerType === kCornerSouthWest || startCorner.cornerType === kCornerSouthEast || goalCorner.cornerType === kCornerNorthWest || goalCorner.cornerType === kCornerNorthEast) {
          return false;
        }
      }
    }
    f = 0;
    sy = 0;
    sx = 0;
    x_offset = 0;
    y_offset = 0;
    if (dy < 0) {
      dy = -dy;
      sy = -1;
      y_offset = 0;
    } else {
      sy = 1;
      y_offset = 1;
    }
    if (dx < 0) {
      dx = -dx;
      sx = -1;
      x_offset = 0;
    } else {
      sx = 1;
      x_offset = 1;
    }
    if (dx >= dy) {
      while (x1 !== x2) {
        f = f + dy;
        if (f >= dx) {
          if (!IsTraversable(x1 + x_offset, y1 + y_offset)) {
            return false;
          }
          y1 = y1 + sy;
          f = f - dx;
        }
        if (f !== 0) {
          if (!IsTraversable(x1 + x_offset, y1 + y_offset)) {
            return false;
          }
        }
        if (dy === 0) {
          if (!IsTraversable(x1 + x_offset, y1) && !IsTraversable(x1 + x_offset, y1 + 1)) {
            return false;
          }
        }
        x1 += sx;
        if (f === 0 && x1 !== x2 && CornerAtDiagonalIntersection(x1, y1)) {
          return false;
        }
      }
    } else {
      while (y1 !== y2) {
        f = f + dx;
        if (f >= dy) {
          if (!IsTraversable(x1 + x_offset, y1 + y_offset)) {
            return false;
          }
          x1 = x1 + sx;
          f = f - dy;
        }
        if (f !== 0) {
          if (!IsTraversable(x1 + x_offset, y1 + y_offset)) {
            return false;
          }
        }
        if (dx === 0) {
          if (!IsTraversable(x1, y1 + y_offset) && !IsTraversable(x1 + 1, y1 + y_offset)) {
            return false;
          }
        }
        y1 += sy;
        if (f === 0 && y1 !== y2 && CornerAtDiagonalIntersection(x1, y1)) {
          return false;
        }
      }
    }
    return true;
  };

    AStarSearch = (function() {
    function AStarSearch(heuristic) {
      this.Heuristic = heuristic;
      this.OpenList = [];
      this.ClosedList = [];
    }

    AStarSearch.prototype.smoothPath = function(path) {
      var corner, i, len, smoothedPath;
      smoothedPath = [];
      if (!path) {
        return path;
      }
      smoothedPath.push(path[0]);
      for (i = 0, len = path.length; i < len; i++) {
        corner = path[i];
        if (!(LineOfSight(smoothedPath[smoothedPath.length - 1], corner))) {
          smoothedPath.push(corner);
        }
      }
      smoothedPath.push(path[path.length - 1]);
      return smoothedPath;
    };

    AStarSearch.prototype.addToClosed = function(node) {
      var corner, i, inClosed, inListNode, indexOfNode, indexToRemove, j, k, len, len1, len2, num, ourFval, ref, ref1, ref2, results;
      this.ClosedList.push(node);
      indexToRemove = this.OpenList.indexOf(node);
      if (!(indexToRemove < 0)) {
        this.OpenList.splice(indexToRemove, 1);
      }
      ref = node.getSuccessors();
      results = [];
      for (i = 0, len = ref.length; i < len; i++) {
        corner = ref[i];
        inClosed = false;
        ref1 = this.ClosedList;
        for (j = 0, len1 = ref1.length; j < len1; j++) {
          inListNode = ref1[j];
          if (inListNode.x === corner.x) {
            if (inListNode.y === corner.y) {
              inClosed = true;
              break;
            }
          }
        }
        if (inClosed) {
          continue;
        }
        indexOfNode = -1;
        num = 0;
        ref2 = this.OpenList;
        for (k = 0, len2 = ref2.length; k < len2; k++) {
          inListNode = ref2[k];
          if (inListNode.x === corner.x) {
            if (inListNode.y === corner.y) {
              indexOfNode = num;
              break;
            }
          }
          num += 1;
        }
        if (indexOfNode < 0) {
          corner.gVal = node.gVal + 1;
          if (this.Heuristic === "EuclideanDistance") {
            corner.hVal = EuclideanDistance(corner, this.Goalcorner);
          } else if (this.Heuristic === "OctileDistance") {
            corner.hVal = OctileDistance(corner, this.Goalcorner);
          } else {
            corner.hVal = ManhattanDistance(corner, this.Goalcorner);
          }
          corner.fVal = corner.gVal + corner.hVal;
          corner.Parent = node;
          results.push(this.OpenList.push(corner));
        } else {
          ourFval = node.gVal + 1 + EuclideanDistance(corner, this.Goalcorner);
          if (ourFval < this.OpenList[indexOfNode]) {
            this.OpenList.splice(indexOfNode, 1);
            corner.gVal = node.gVal + 1;
            if (this.Heuristic === "EuclideanDistance") {
              corner.hVal = EuclideanDistance(corner, this.Goalcorner);
            } else if (this.Heuristic === "OctileDistance") {
              corner.hVal = OctileDistance(corner, this.Goalcorner);
            } else {
              corner.hVal = ManhattanDistance(corner, this.Goalcorner);
            }
            corner.fVal = corner.gVal + corner.hVal;
            corner.Parent = node;
            results.push(this.OpenList.push(corner));
          } else {
            results.push(void 0);
          }
        }
      }
      return results;
    };

    AStarSearch.prototype.search = function(startcorner, goalcorner) {
      var curr, currNode, i, len, minFval, node, path, reachedGoal, ref;
      this.Goalcorner = goalcorner;
      path = [];
      if (startcorner === goalcorner) {
        console.log("Start is Goal");
        return path;
      }
      startcorner.gVal = 0;
      if (this.Heuristic === "EuclideanDistance") {
        startcorner.hVal = EuclideanDistance(startcorner, goalcorner);
        startcorner.fVal = EuclideanDistance(startcorner, goalcorner);
      } else if (this.Heuristic === "OctileDistance") {
        startcorner.hVal = OctileDistance(startcorner, goalcorner);
        startcorner.fVal = OctileDistance(startcorner, goalcorner);
      } else {
        startcorner.hVal = ManhattanDistance(startcorner, goalcorner);
        startcorner.fVal = ManhattanDistance(startcorner, goalcorner);
      }
      this.addToClosed(startcorner);
      reachedGoal = false;
      while (this.OpenList.length) {
        minFval = 9999999999;
        currNode = null;
        ref = this.OpenList;
        for (i = 0, len = ref.length; i < len; i++) {
          node = ref[i];
          if (node.fVal < minFval) {
            minFval = node.fVal;
            currNode = node;
          }
        }
        if (currNode) {
          this.addToClosed(currNode);
          if (currNode === goalcorner) {
            console.log("Reached goal!");
            reachedGoal = true;
            break;
          }
        } else {
          console.log("Open list empty");
          break;
        }
      }
      if (reachedGoal) {
        curr = goalcorner;
        while (curr !== startcorner) {
          path.push(curr);
          curr = curr.Parent;
        }
        path.push(startcorner);
      }
      path = path.reverse();
      path = this.smoothPath(path);
      return path;
    };

    return AStarSearch;

  })();
    
      var mapstring = []; //this array of strings represents the map

      function checkCase(y,x){
//          Cell = (function() {
//            function Cell(isObstacle) {
//              this.isObstacle = isObstacle;
//              this.northWest = null;
//              this.northEast = null;
//              this.southWest = null;
//              this.southEast = null;
//            }
//           return Cell;
//
//  })();

          var northWest = false;
          var northEast = false;
          var southWest = false;
          var southEast = false;
          var result = [];
          //check northWest Block
          if(x == 0 || y == 0){
            northWest = true;
          }else{
            if($scope.cell[y- 1][x - 1].isObstacle){
                northWest = true;
            }
          }

          //check northEast Block
          if(y == 0 || x == 30){
            northEast = true;
          }else{
            if($scope.cell[y -1][x].isObstacle){
                northEast = true;
            }
          }

          //check southWest Block
          if(x == 0 || y == 20){
            southWest = true;
          }else{
            if($scope.cell[y][x - 1].isObstacle){
                southWest = true;
            }
          }
          //check southEast Block
          if(x == 30 || y == 20){
            southEast = true;
          }else{
            if($scope.cell[y][x].isObstacle){
                southEast = true;
            }
          }

          if(!northWest && !northEast && !southWest && !southEast){
            result[0] = 0;
            result[1] = [];
            result[1] = [1,1,1,1];
            return result; // all way normal
          }

          //three way normal
          if(northWest && !northEast && !southWest && !southEast){
             result[0] = 0;
            result[1] = [];
            result[1] = [0,1,1,1];
            return result; //except northwest
          }

          if(!northWest && northEast && !southWest && !southEast){
             result[0] = 0;
            result[1] = [];
            result[1] = [1,0,1,1];
            return result; //except northeast
          }
          if (!northWest && !northEast && southWest && !southEast){
             result[0] = 0;
            result[1] = [];
            result[1] = [1,1,1,0];
            return result; //except southwest
          }
          if(!northWest && !northEast && !southWest && southEast){
             result[0] = 0;
            result[1] = [];
            result[1] = [1,1,0,1];
            return result; //except southeast
          }


          //two way normal
          if(northWest && northEast && !southWest && !southEast){
            result[0] = 0;
            result[1] = [];
            result[1] = [0,0,1,1];
            return result; //except northwest and east
          }
          if(!northWest && northEast && !southWest && southEast){
                result[0] = 0;
            result[1] = [];
            result[1] = [1,0,0,1];
            return result; //except northeast and southeast
          }
          if(!northWest && !northEast && southWest && southEast){
               result[0] = 0;
            result[1] = [];
            result[1] = [1,1,0,0];
            return result; //except southwest and southeast
          }

          if(northWest && !northEast && southWest && !southEast){
            result[0] = 0;
            result[1] = [];
            result[1] = [0,1,1,0];
            return result; //except southwest and northwest
          }


          //one corner
          if(northWest && northEast && southWest && !southEast){
            result[0] = 1;  // one corner
            result[1] = [];
            result[1] = [0,0,1,0];
            return result;
          }
          if (northWest && northEast && !southWest && southEast){
               result[0] = 1;  // one corner
            result[1] = [];
            result[1] = [0,0,0,1];
            return result;
          }
          if(!northWest && northEast && southWest && southEast){
              result[0] = 1;  // one corner
            result[1] = [];
            result[1] = [1,0,0,0];
            return result;
          }
          if (northWest && !northEast && southWest && southEast){
             result[0] = 1;  // one corner
            result[1] = [];
            result[1] = [0,1,0,0];
            return result;
          }


        //two corners
        if(!northWest && northEast && southWest && !southEast){
          result[0] = 2;  // two corners
            result[1] = [];
            result[1] = [1,0,1,0];
            return result;
        }
        if(northWest && !northEast && !southWest && southEast){
             result[0] = 2;  // two corners
            result[1] = [];
            result[1] = [0,1,0,1];
            return result;
        }

      };

      function checkNeighborhood(corner){
        var x = corner.x;
        var y = corner.y;
        //check not normal corner cases
        if(corner.cornerType == "northWest"){
          //south
          for(var k = 0; k < $scope.cornersMap[y + 1][x].length; k++){
            if($scope.cornersMap[y + 1][x][k].type == "southWest" || $scope.cornersMap[y + 1][x][k].type == "normal"){
              corner.South = $scope.cornersMap[y + 1][x][k];
            }
          }
          //east
          for(var k = 0; k < $scope.cornersMap[y][x + 1].length; k++){
            if($scope.cornersMap[y][x + 1][k].cornerType == "northEast" || $scope.cornersMap[y][x + 1][k].cornerType == "normal"){
              corner.East = $scope.cornersMap[y][x + 1][k];
            }
          }
          //southeast
          for(var k = 0; k < $scope.cornersMap[y + 1][x + 1].length; k++){
            if($scope.cornersMap[y + 1][x + 1][k].cornerType == "southEast" || $scope.cornersMap[y + 1][x + 1][k].cornerType == "normal"){
              corner.SouthEast = $scope.cornersMap[y + 1][x + 1][k];
            }
          }
        }
        else if(corner.cornerType == "northEast"){
          //west
          for(var k = 0; k < $scope.cornersMap[y][x - 1].length; k++){
            if($scope.cornersMap[y][x - 1][k].cornerType == "northWest" || $scope.cornersMap[y][x - 1][k].cornerType == "normal"){
              corner.West = $scope.cornersMap[y][x - 1][k];
            }
          }
          //south
          for(var k = 0; k < $scope.cornersMap[y + 1][x].length; k++){
            if($scope.cornersMap[y + 1][x][k].cornerType == "southEast" || $scope.cornersMap[y + 1][x][k].cornerType == "normal"){
              corner.South = $scope.cornersMap[y + 1][x][k];
            }
          }
          //southwest
          for(var k = 0; k < $scope.cornersMap[y + 1][x - 1].length; k++){
            if($scope.cornersMap[y + 1][x - 1][k].cornerType == "southWest" || $scope.cornersMap[y + 1][x - 1][k].cornerType == "normal"){
              corner.SouthWest = $scope.cornersMap[y + 1][x - 1][k];
            }
          }
        }
        else if(corner.cornerType == "southEast"){
          //north
          for(var k = 0; k < $scope.cornersMap[y - 1][x].length; k++){
            if($scope.cornersMap[y - 1][x][k].cornerType == "northEast" || $scope.cornersMap[y - 1][x][k].cornerType == "normal"){
              corner.North = $scope.cornersMap[y - 1][x][k];
            }
          }
          //northWest
          for(var k = 0; k < $scope.cornersMap[y - 1][x - 1].length; k++){
            if($scope.cornersMap[y - 1][x - 1][k].cornerType == "northWest" || $scope.cornersMap[y - 1][x - 1][k].cornerType == "normal"){
              corner.NorthWest = $scope.cornersMap[y - 1][x - 1][k];
            }
          }
          //west
          for(var k = 0; k < $scope.cornersMap[y][x - 1].length; k++){
            if($scope.cornersMap[y][x - 1][k].cornerType == "southWest" || $scope.cornersMap[y][x - 1][k].cornerType == "normal"){
              corner.West = $scope.cornersMap[y][x - 1][k];
            }
          }
          
        }
        else if(corner.cornerType == "southWest"){
          //north
          for(var k = 0; k < $scope.cornersMap[y - 1][x].length; k++){
            if($scope.cornersMap[y - 1][x][k].cornerType == "northWest" || $scope.cornersMap[y - 1][x][k].cornerType == "normal"){
              corner.NorthWest = $scope.cornersMap[y - 1][x][k];
            }
          }
          //northEast
          for(var k = 0; k < $scope.cornersMap[y - 1][x + 1].length; k++){
            if($scope.cornersMap[y - 1][x + 1][k].type == "northEast" || $scope.cornersMap[y - 1][x + 1][k].type == "normal"){
              corner.northEast = $scope.cornersMap[y - 1][x + 1][k];
            }
          }
          //east
          for(var k = 0; k < $scope.cornersMap[y][x + 1].length; k++){
            if($scope.cornersMap[y][x + 1][k].type == "southEast" || $scope.cornersMap[y][x + 1][k].type == "normal"){
              corner.east = $scope.cornersMap[y][x + 1][k];
            }
          }
        }
        else if(corner.type == "normal"){

          var northWest = false;
          var northEast = false;
          var southWest = false;
          var southEast = false;
          //check northWest Block
          if(x == 0 || y == 0){
            northWest = true;
          }else{
            if($scope.cell[y- 1][x - 1].isObstacle){
                northWest = true;
            }
          }
          //check northEast Block
          if(y == 0 || x == 30){
            northEast = true;
          }else{
            if($scope.cell[y -1][x].isObstacle){
                northEast = true;
            }
          }
          //check southWest Block
          if(x == 0 || y == 20){
            southWest = true;
          }else{
            if($scope.cell[y][x - 1].isObstacle){
                southWest = true;
            }
          }
          //check southEast Block
          if(x == 30 || y == 20){
            southEast = true;
          }else{
            if($scope.cell[y][x].isObstacle){
                southEast = true;
            }
          }
          var addNorthWest = false;
          var addNorth = false;
          var addNorthEast = false;
          var addEast = false;
          var addSouthEast = false;
          var addSouth = false;
          var addSouthWest = false;
          var addWest = false;

          //all way case
          if(!northWest && !northEast && !southWest && !southEast){
            addNorthWest = true;
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addSouthEast = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }
          //three way normal
          if(northWest && !northEast && !southWest && !southEast){
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addSouthEast = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }
          if(!northWest && northEast && !southWest && !southEast){
            addNorthWest = true;
            addNorth = true;
            addEast = true;
            addSouthEast = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }
          if (!northWest && !northEast && southWest && !southEast){
            addNorthWest = true;
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addSouthEast = true;
            addSouth = true;
            addWest = true;
          }
          if(!northWest && !northEast && !southWest && southEast){
            addNorthWest = true;
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }


          //two way normal
          if(northWest && northEast && !southWest && !southEast){
            addEast = true;
            addSouthEast = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }
          if(!northWest && northEast && !southWest && southEast){
            addNorthWest = true;
            addNorth = true;
            addSouth = true;
            addSouthWest = true;
            addWest = true;
          }
          if(!northWest && !northEast && southWest && southEast){
            addNorthWest = true;
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addWest = true;
          }
          if(northWest && !northEast && southWest && !southEast){
            addNorth = true;
            addNorthEast = true;
            addEast = true;
            addSouthEast = true;
            addSouth = true;
          }


          if(addNorthWest){
             //add northWest
            for(var k = 0; k < $scope.cornersMap[y - 1][x - 1].length; k++){
              if($scope.cornersMap[y - 1][x - 1][k].type == "northWest" || $scope.cornersMap[y - 1][x - 1][k].type == "normal"){
                corner.northWest = $scope.cornersMap[y - 1][x - 1][k];
              }
            }
          }
          if(addNorth){
            //add north
            for(var k = 0; k < $scope.cornersMap[y - 1][x].length; k++){
              if($scope.cornersMap[y - 1][x][k].type == "northWest" || $scope.cornersMap[y - 1][x][k].type == "northEast" ||$scope.cornersMap[y - 1][x][k].type == "normal"){
                corner.northWest = $scope.cornersMap[y - 1][x][k];
              }
            }
          }
          if(addNorthEast){
            //add northEast
            for(var k = 0; k < $scope.cornersMap[y - 1][x + 1].length; k++){
              if($scope.cornersMap[y - 1][x + 1][k].type == "northEast" || $scope.cornersMap[y - 1][x + 1][k].type == "normal"){
                corner.northEast = $scope.cornersMap[y - 1][x + 1][k];
              }
            }
          }
          if(addEast){
            //add east
            for(var k = 0; k < $scope.cornersMap[y][x + 1].length; k++){
              if($scope.cornersMap[y][x + 1][k].type == "northEast" || $scope.cornersMap[y][x + 1][k].type == "southEast" || $scope.cornersMap[y][x + 1][k].type == "normal"){
                corner.east = $scope.cornersMap[y][x + 1][k];
              }
            }
          }
          if(addSouthEast){
            //add southEast
            for(var k = 0; k < $scope.cornersMap[y + 1][x + 1].length; k++){
              if($scope.cornersMap[y + 1][x + 1][k].type == "southEast" || $scope.cornersMap[y + 1][x + 1][k].type == "normal"){
                corner.southEast = $scope.cornersMap[y + 1][x + 1][k];
              }
            }
          }
          if(addSouth){
            //add south
            for(var k = 0; k < $scope.cornersMap[y + 1][x].length; k++){
              if($scope.cornersMap[y + 1][x][k].type == "southEast" || $scope.cornersMap[y + 1][x][k].type == "southWest" || $scope.cornersMap[y + 1][x][k].type == "normal"){
                corner.south = $scope.cornersMap[y + 1][x][k];
              }
            }
          }
          if(addSouthWest){
            //add southWest
            for(var k = 0; k < $scope.cornersMap[y + 1][x - 1].length; k++){
              if($scope.cornersMap[y + 1][x - 1][k].type == "southWest" || $scope.cornersMap[y + 1][x - 1][k].type == "normal"){
                corner.southWest = $scope.cornersMap[y + 1][x - 1][k];
              }
            }
          }
          if(addWest){
            //add west
            for(var k = 0; k < $scope.cornersMap[y][x - 1].length; k++){
              if($scope.cornersMap[y][x - 1][k].type == "southWest" || $scope.cornersMap[y][x - 1][k].type == "northWest" ||$scope.cornersMap[y][x - 1][k].type == "normal"){
                corner.west = $scope.cornersMap[y][x - 1][k];
              }
            }
          }
        }
      };

    (function() {
            function Corner(x, y, cornerType) {
                this.x = x;
                this.y = y;
                this.cornerType = cornerType;
            }

            Corner.North = null;

            Corner.NorthWest = null;

            Corner.NorthEast = null;

            Corner.East = null;

            Corner.West = null;

            Corner.South = null;

            Corner.SouthWest = null;

            Corner.SouthEast = null;

            Corner.fVal = 999999999999999999;

            Corner.gVal = 999999999999999999;

            Corner.hVal = 999999999999999999;

            Corner.Parent = null;

            Corner.prototype.getSuccessors = function() {
                var successors;
                successors = [];
                if (this.North) {
                    successors.push(this.North);
                }   
                if (this.NorthWest) {
                    successors.push(this.NorthWest);
                }
                if (this.NorthEast) {
                    successors.push(this.NorthEast);
                }
                if (this.West) {
                    successors.push(this.West);
                }
                if (this.East) {
                    successors.push(this.East);
                }
                if (this.SouthWest) {
                    successors.push(this.SouthWest);
                }
                if (this.SouthEast) {
                    successors.push(this.SouthEast);
                }
                if (this.South) {
                    successors.push(this.South);
                }
                return successors;
            };

            return Corner;
        })();
      function createCorner(y,x,type){
//        var Corner = 
//          x: x,
//          y: y,
//          cornerType: type,
//          NorthWest: null,
//          North: null,
//          NorthEast: null,
//          East: null,
//          SouthEast: null,
//          South: null,
//          SouthWest: null,
//          West: null,
//          fVal: 999999999999999999,
//          gVal: 999999999999999999,
//          hVal: 999999999999999999,
//          Parent: null
//        };
//        return cornerObject;
        var Corner = new Corner(x,y,type);
        
      };




      $scope.submit = function(){
          var temp;
          for(var i = 0; i < $scope.size.row + 1; i ++){
            for(var j = 0; j < $scope.size.column + 1; j++){
                temp = checkCase(i,j);

                //if normal corner
                if(temp[0] == 0){
                    var cornerObj = createCorner(i,j,"normal");
                    $scope.cornersMap[i][j].push(cornerObj);
                    $scope.corners.push(cornerObj);
                    for(var k = 0; k < temp[1].length; k++){
                      if(temp[1][k] == 1){
                        if(k == 0){
                          //northwest cell
                          $scope.cell[i - 1][j - 1].corners[$scope.cell[i - 1][j - 1].corners.length] = cornerObj;
                        }
                        if(k == 1){
                          //northeast cell
                          $scope.cell[i - 1][j].corners[$scope.cell[i - 1][j].corners.length] = cornerObj;
                        }
                        if(k == 2){
                          //southeast cell
                          $scope.cell[i][j].corners[$scope.cell[i][j].corners.length] = cornerObj;
                        }
                        if(k == 3){
                          //southwest cell
                          $scope.cell[i][j - 1].corners[$scope.cell[i][j - 1].corners.length] = cornerObj;
                        }
                      }
                    }
                }

                //if one corner
                if(temp[0] == 1){
                  for(var k = 0; k < temp[1].length; k++){
                      if(temp[1][k] == 1){
                        if(k == 0){
                          //northwest cell
                          var cornerObj = createCorner(i,j,"southEast");
                          $scope.cell[i - 1][j - 1].corners[$scope.cell[i - 1][j - 1].corners.length] = cornerObj;
                        }
                        if(k == 1){
                          //northeast cell
                          var cornerObj = createCorner(i,j,"southWest");
                          $scope.cell[i - 1][j].corners[$scope.cell[i - 1][j].corners.length] = cornerObj;
                        }
                        if(k == 2){
                          //southeast cell
                          var cornerObj = createCorner(i,j,"northWest");
                          $scope.cell[i][j].corners[$scope.cell[i][j].corners.length] = cornerObj;
                        }
                        if(k == 3){
                          //southwest cell
                          var cornerObj = createCorner(i,j,"northEast");
                          $scope.cell[i][j - 1].corners[$scope.cell[i][j - 1].corners.length] = cornerObj;
                        }
                        $scope.cornersMap[i][j].push(cornerObj);
                          $scope.corners.push(cornerObj);
                      }
                    }
                }

                // two corners special case
                 if(temp[0] == 2){
                  for(var k = 0; k < temp[1].length; k++){
                      if(temp[1][k] == 1){
                        if(k == 0){
                          //northwest cell
                          var cornerObj = createCorner(i,j,"southEast");
                          $scope.cell[i - 1][j - 1].corners[$scope.cell[i - 1][j - 1].corners.length] = cornerObj;
                        }
                        if(k == 1){
                          //northeast cell
                          var cornerObj = createCorner(i,j,"southWest");
                          $scope.cell[i - 1][j].corners[$scope.cell[i - 1][j].corners.length] = cornerObj;
                        }
                        if(k == 2){
                          //southeast cell
                          var cornerObj = createCorner(i,j,"northWest");
                          $scope.cell[i][j].corners[$scope.cell[i][j].corners.length] = cornerObj;
                        }
                        if(k == 3){
                          //southwest cell
                          var cornerObj = createCorner(i,j,"northEast");
                          $scope.cell[i][j - 1].corners[$scope.cell[i][j - 1].corners.length] = cornerObj;
                        }
                        $scope.cornersMap[i][j].push(cornerObj);
                          $scope.corners.push(cornerObj);
                      }
                    }
                }

            }
          }


          //load each corner's neighborhoods
          for(var i = 0; i < $scope.size.row + 1; i ++){
            for(var j = 0; j < $scope.size.column + 1; j++){
              for(var o = 0; o < $scope.cornersMap[i][j].length; o++){
                checkNeighborhood($scope.cornersMap[i][j][o]);
              }       
            }
          }
          console.log("done");
          
          $scope.search();
      };


      $scope.search = function(){
          myGrid = $scope.cell;
          searchie = new AStarSearch("OctileDistance");

          path = searchie.search(Corners[4], Corners[17]);

          console.log("Pathie");

          for (i = 0, len = path.length; i < len; i++) {
            corner = path[i];
            console.log(corner.x, corner.y);
          }
      };



      $scope.tempPath = [
      [0,20],
      [0,19],
      [2,18],
       [3,18],
      [3,17],
      [4,15],
       [6,17],
      [8,14],
      [10,13],
       [11,12],
      [12,10],
      [15,7],
      [17,7],
      [18,6],
      [21,5],
      [25,3],
      [26,3],
      [27,2],
      [28,2],
      [30,1]
      ];

      $scope.tempPath2 = [
      [0,20],
      [4,19],
      [30,1]
      ];

       $scope.tempPath3 = [
      [0,20],
      [6,16],
      [15,10],
      [30,1]
      ];


      $scope.drawPath = function(){

            for(var i = 0 ; i <  $scope.tempPath.length - 1 ; i++){
              lines[0].moveTo($scope.tempPath[i][0] * unit ,$scope.tempPath[i][1] * unit);
              lines[0].lineTo($scope.tempPath[i + 1][0] * unit,$scope.tempPath[i + 1][1] * unit);
            }
            lines[0].stroke();
      };

      $scope.pathOne = function(){
              for(var i = 0 ; i <  $scope.tempPath.length - 1 ; i++){
              lines[0].moveTo($scope.tempPath[i][0] * unit ,$scope.tempPath[i][1] * unit);
              lines[0].lineTo($scope.tempPath[i + 1][0] * unit,$scope.tempPath[i + 1][1] * unit);
            }
            lines[0].stroke();
      };

       $scope.pathTwo = function(){
              for(var i = 0 ; i <  $scope.tempPath2.length - 1 ; i++){
              lines[1].moveTo($scope.tempPath2[i][0] * unit ,$scope.tempPath2[i][1] * unit);
              lines[1].lineTo($scope.tempPath2[i + 1][0] * unit,$scope.tempPath2[i + 1][1] * unit);
            }
            lines[1].stroke();
      };

      $scope.pathThree = function(){
              for(var i = 0 ; i <  $scope.tempPath3.length - 1 ; i++){
              lines[2].moveTo($scope.tempPath3[i][0] * unit ,$scope.tempPath3[i][1] * unit);
              lines[2].lineTo($scope.tempPath3[i + 1][0] * unit,$scope.tempPath3[i + 1][1] * unit);
            }
            lines[2].stroke();
      };

      $scope.clearPath = function(index){
           lines[index].clearRect(0,0,canvases[index].width,canvases[index].height);
      };


});