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
  });