/*
 * Module dependencies
 */
var express = require('express'),
    stylus = require('stylus'),
    nib = require('nib')
    var http = require("http");
var https = require("https");

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

app.listen(3000)

app.post("/RunAStar", function(req, res) {
    child = exec('bash runner', // command line argument directly in string
        function(error, stdout, stderr) { // one easy function to capture data/errors
            console.log('stdout: ' + stdout);
            console.log('stderr: ' + stderr);
            if (error !== null) {
                console.log('exec error: ' + error);
            }
        });
})