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
          $scope.cell[i][j].obstacle = false;
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
       $scope.corners = {};
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
            $scope.cell[row][column].obstacle = !$scope.cell[row][column].obstacle;
        }

      };



      var mapstring = []; //this array of strings represents the map



      document.getElementById("searchButton").addEventListener("click", handler)

      // for(var i = 0 ; i < $scope.size.row; i ++){
      //   for (var j = 0; j < $scope.size.column; j++){
      //     //cornercase
      //     if(i == 0 && j == 0){
      //       var upperLeft = i + "" + j;
      //       $scope.cell[i][j].corners.push(upperLeft);
      //     }
      //     if(i == 0 && j == 20){
      //       var lowerLeft = i + "" + (j + 1);
      //       $scope.cell[i][j].corners.push(lowerLeft);
      //     }
      //     if(i == 30 && j == 0){
      //       var upperRight = (i + 1) + "" + j;
      //       $scope.cell[i][j].corners.push(upperRight);
      //     }
      //     if(i == 30 && j = 20){
      //       var lowerRight = (i + 1) + "" + (j + 1);
      //       $scope.cell[i][j].corners.push(lowerRight);
      //     }
      //   }
      // }


      // var size = {
      //   row:22
      // };
      // function handler(e) {
      //   $scope.drawPath();
      //   for(var i = 0; i < size.row; i++)
      //   {
      //     var str = "@"; //it's bordered by @ on all sides
      //     if(i == 0 || i > 20)
      //     {
      //         for(var q = 1; q < 31; q++)
      //         {
      //           str = str.concat("@");
      //         }
      //     }
      //             else
      //             {
      //               for(var j = 0; j < $scope.size.column; j++)
      //               {

      //                 if($scope.cell[i-1][j] == false)
      //                 {
      //                   str = str.concat(".");
      //                 } else str = str.concat("@");

      //             }
      //   }
      //     str = str.concat("@");
      //     mapstring[i] = str;
      // }
      //   var payload = {
      //     map: mapstring
      //   };
      //   console.log(payload.map);


         // $http({
         //        method: 'POST',
         //        url: 'http://localhost:3000/RunAStar',
         //        data: payload,
         //        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
         //        transformRequest: function(obj) {
         //                  var str = [];
         //                  for(var p in obj){
         //                      str.push(encodeURIComponent(p) + '=' + encodeURIComponent(obj[p]));
         //                  }
         //                  return str.join('&');
         //                  }
         //   }).success(function(response) {

         //  });

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