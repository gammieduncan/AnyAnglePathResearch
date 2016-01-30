angular.module('anyAngle', [])
  .controller('mainController', function($scope) {

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

      /*function searchingGrid(){ }

      searchingGrid.prototype = {

        constructor: searchingGrid,

        toSearch : function(event) {

          event.preventDefault(); */

    //document.getElementById("searchButton").addEventListener("click", function(event)

      document.getElementById("searchButton").addEventListener("click", handler)
      
      function handler(e) {
        
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

        

  

  });