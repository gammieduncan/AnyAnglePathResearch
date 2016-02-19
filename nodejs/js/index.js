angular.module('anyAngle', ['ngMaterial'])
    .controller('mainController', function($scope, $http) {
        var c = document.getElementById("mainCanvas");
        var ctx = c.getContext("2d");
        var unit = 6.66666;

        ctx.lineWidth = 10;
        ctx.beginPath();


        $scope.size = {
            row: 20,
            column: 30
        };
        $scope.gridStatus = [];
        for (var i = 0; i < $scope.size.row; i++) {
            $scope.gridStatus[i] = [];
            for (var j = 0; j < $scope.size.column; j++) {
                $scope.gridStatus[i][j] = false;
            }
        }
        $scope.getNumber = function(num) {
            return new Array(num);
        };
        $scope.gridClicked = function(row, column) {
            if (!(row == 19 && column == 0) && !(row == 0 && column == 29)) {
                $scope.gridStatus[row][column] = !$scope.gridStatus[row][column];
            }

        };

        var mapstring = []; //this array of strings represents the map



        document.getElementById("searchButton").addEventListener("click", handler)
        var size = {
            row: 22
        };

        function handler(e) {
            $scope.drawPath();
            for (var i = 0; i < size.row; i++) {
                var str = "@"; //it's bordered by @ on all sides
                if (i == 0 || i > 20) {
                    for (var q = 1; q < 31; q++) {
                        str = str.concat("@");
                    }
                } else {
                    for (var j = 0; j < $scope.size.column; j++) {

                        if ($scope.gridStatus[i - 1][j] == false) {
                            str = str.concat(".");
                        } else str = str.concat("@");

                    }
                }
                str = str.concat("@");
                mapstring[i] = str;
            }
            console.log(mapstring);
        
            var payload = {
                map: mapstring
            };
            $http({
                method: 'POST',
                url: 'http://localhost:3000/RunAStar',
                data: payload,
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                transformRequest: function(obj) {
                    var str = [];
                    for (var p in obj) {
                        str.push(encodeURIComponent(p) + '=' + encodeURIComponent(obj[p]));
                    }
                    return str.join('&');
                }
            }).success(function(response) {

            });

        };





        $scope.tempPath = [
            [0, 20],
            [0, 17],
            [2, 14],
            [10, 9],
            [16, 6],
            [25, 3],
            [30, 1]
        ];


        $scope.drawPath = function() {

            for (var i = 0; i < $scope.tempPath.length - 1; i++) {
                ctx.moveTo($scope.tempPath[i][0] * unit, $scope.tempPath[i][1] * unit);
                ctx.lineTo($scope.tempPath[i + 1][0] * unit, $scope.tempPath[i + 1][1] * unit);

            }
            ctx.stroke();
        };


    });