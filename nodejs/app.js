/*
 * Module dependencies
 */
var express = require('express'),
    stylus = require('stylus'),
    nib = require('nib')
    var http = require("http");
var https = require("https");
var bodyParser = require("body-parser");

var app = express()

    function compile(str, path) {
        return stylus(str)
            .set('filename', path)
            .use(nib())
    }
app.set('views', __dirname + '/views')
app.set('view engine', 'jade')
app.use(express.logger('dev'))
app.use(stylus.middleware({
    src: __dirname + '/public',
    compile: compile
}))
app.use(express.static(__dirname + '/public'))
app.use(bodyParser.json({limit: '5mb'}));
app.use(bodyParser.urlencoded({limit:'50mb'}));
app.get('/', function(req, res) {
    res.render('index', {
        title: 'Home'
    })
})

var util = require('util'),
    exec = require('child_process').exec,
    child;

child = exec('chmod +x runner', // command line argument directly in string
    function(error, stdout, stderr) { // one easy function to capture data/errors
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        if (error !== null) {
            console.log('exec error: ' + error);
        }
    });

app.listen(3000);

app.post("/RunAStar", function(req, res) {
      var mapArray = req.body.map.split(",");
    var mapText = "";
    mapText += "type octile\nheight 22\nwidth 32\nmap\n";
    for(var i = 0; i < mapArray.length; i++ ){
        if(i != mapArray.length - 1){
           mapText += mapArray[i] + "\n";  
        }else{
            mapText += mapArray[i];
        }
       
        
    }

      console.log(mapText);
    var fs = require('fs');
    fs.writeFile("arena2.map", mapText, function(err) {
        if (err) {
            return console.log(err);
        }

        console.log("The file was saved!");
    });
    child = exec('bash runner', // command line argument directly in string
        function(error, stdout, stderr) { // one easy function to capture data/errors
            console.log('stdout: ' + stdout);
            console.log('stderr: ' + stderr);
            if (error !== null) {
                console.log('exec error: ' + error);
            }
            res.setHeader('Access-Control-Allow-Origin', '*');
            res.send({
                result: stdout
            });
        });
})