var lines = [];
var canvases = [];

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

      for(var i = 0 ; i < $scope.size.row; i ++){
          $scope.cell[i] = [];
        for (var j = 0; j < $scope.size.column ; j++){
          $scope.cell[i][j] = {};
          $scope.cell[i][j].isObstacle = false;
          $scope.cell[i][j].corners = [];
          //upperLeft corner
          //var upperLeft = i + "" + j;
          //upperRight corner
          //var upperRight = (i + 1) + "" + j;
          //lowerLeft corner
          //var lowerLeft = i + "" + (j + 1);
          //lowerRight corner
          // var lowerRight = (i + 1) + "" + (j + 1);
          // $scope.cell[i][j].corners.push(upperLeft);
          // $scope.cell[i][j].corners.push(upperRight);
          // $scope.cell[i][j].corners.push(lowerLeft);
          // $scope.cell[i][j].corners.push(lowerRight);
        }
      }
       $scope.corners = [];
      //init corner
      // for(var i = 0 ; i < $scope.size.row + 1; i ++){
      //   for (var j = 0; j < $scope.size.column + 1; j++){
      //     var cornerId = i + "" + j;
      //     $scope.corners[cornerId] = {};
      //     $scope.corners[cornerId].x = i;
      //     $scope.corners[cornerId].y = j;
      //     $scope.corners[cornerId].y = j;
      //   }
      // }


      $scope.getNumber = function(num) {
          return new Array(num);
      };
      $scope.gridClicked = function(row, column){
        if(!(row == 19 && column == 0) && !(row == 0 && column == 29)){
            $scope.cell[row][column].isObstacle = !$scope.cell[row][column].isObstacle;
        }

      };



      var mapstring = []; //this array of strings represents the map

      function checkCase(y,x){
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

      function createCorner(y,x,type){
        var cornerObject = {
          x: x,
          y: y,
          type: type
        };
        return cornerObject;
      };


      $scope.submit = function(){
          var temp;
          for(var i = 0; i < $scope.size.row + 1; i ++){
            for(var j = 0; j < $scope.size.column + 1; j++){
                temp = checkCase(i,j);

                //if normal corner
                if(temp[0] == 0){
                    var cornerObj = createCorner(i,j,"normal");
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
                          var cornerObj = createCorner(i,j,"southWest");
                          $scope.cell[i - 1][j - 1].corners[$scope.cell[i - 1][j - 1].corners.length] = cornerObj;
                        }
                        if(k == 1){
                          //northeast cell
                          var cornerObj = createCorner(i,j,"southEast");
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
                      }
                    }
                }

                // two corners special case
                 if(temp[0] == 2){
                  for(var k = 0; k < temp[1].length; k++){
                      if(temp[1][k] == 1){
                        if(k == 0){
                          //northwest cell
                          var cornerObj = createCorner(i,j,"southWest");
                          $scope.cell[i - 1][j - 1].corners[$scope.cell[i - 1][j - 1].corners.length] = cornerObj;
                        }
                        if(k == 1){
                          //northeast cell
                          var cornerObj = createCorner(i,j,"southEast");
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
                      }
                    }
                }

            }
          }

          console.log("done");
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