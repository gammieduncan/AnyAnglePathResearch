
angular.module('anyAngle', ['ngMaterial'])
  .controller('mainController', function($scope) {
       var c=document.getElementById("mainCanvas");
        var ctx=c.getContext("2d");
        ctx.beginPath();



      $scope.size = {
        row: 20,
        column: 30
      };
      $scope.gridStatus = [];
      for(var i = 0 ; i < $scope.size.row; i ++){
        $scope.gridStatus[i] = [];
        for (var j = 0; j < $scope.size.column ; j++){
          $scope.gridStatus[i][j] = false;
        }
      }
      $scope.getNumber = function(num) {
          return new Array(num);
      };
      $scope.gridClicked = function(row, column){
        if(!(row == 19 && column == 0) && !(row == 0 && column == 29)){
            $scope.gridStatus[row][column] = !$scope.gridStatus[row][column];
        }

      };

      var mapstring = []; //this array of strings represents the map
      mapstring.size = {
        row: 22
        //column: 32
      };


      document.getElementById("searchButton").addEventListener("click", handler)

      function handler(e) {
        $scope.drawPath();
        for(var i = 0; i < mapstring.size.row; i++)
        {
          var str = "@"; //it's bordered by @ on all sides
          if(i == 0 || i > 20)
          {
              for(var q = 1; q < 31; q++)
              {
                str = str.concat("@");
              }
          }
                  else
                  {
                    for(var j = 0; j < $scope.size.column; j++)
                    {

                      if($scope.gridStatus[i-1][j] == false)
                      {
                        str = str.concat(".");
                      } else str = str.concat("@");

                  }
        }
          str = str.concat("@");
          mapstring[i] = str;
      }
        console.log(mapstring);
      };



      $scope.tempPath = [
      [0,0],
      [0,10],
      [0,20],
      [30,50],
      [40,20],
      [60,20],
      [60,30]
      ];


      $scope.drawPath = function(){

            for(var i = 0 ; i <  $scope.tempPath.length - 1 ; i++){
              ctx.moveTo($scope.tempPath[i][0],$scope.tempPath[i][1]);
            ctx.lineTo($scope.tempPath[i + 1][0],$scope.tempPath[i + 1][1]);
            }
            ctx.stroke();
      };




  });