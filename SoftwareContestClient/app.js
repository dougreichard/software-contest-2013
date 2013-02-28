var express = require("express");
var net = require ('net');
var sio = require('socket.io');

var app = express.createServer();
app.use(express.static(__dirname + '/public'));  // Static page location
app.use(app.router);

var io =sio.listen(app);

var eventsocket = 0;

var fs = require('fs');
var file = __dirname + '/configgame.json';
var config = { playerone:"lefty", playertwo:"righty"};

fs.readFile(file, 'utf8', function (err, data) {
    if (err) {
        console.log('Error: ' + err);
        return;
    }

    config = JSON.parse(data);

    console.log(data);
});


//
// Start the HTTP server
//
app.listen(8080);
console.log('App Server RUNNING at http://127.0.0.1:8080');


var t = 0;

io.sockets.on('connection', function(cl) {
    eventsocket = cl;
});


var client = new net.Socket();
client.connect(8124, '127.0.0.1', function () {

    console.log("Connected to game server.");

});


app.get('/cardgame/start', function(req,res){
    //if (client.)

    console.log("Got start request");
    client.once('data', 	function (data) {

        res.setHeader('Cache-Control', "NO-CACHE")
        res.send(data);
        console.log(data.length);

    });
    client.write('{"host":"card"}');
});

app.get('/cardgame/refill', function(req,res){
    //if (client.)

    console.log("Got refill request");
    client.once('data', 	function (data) {

        res.setHeader('Cache-Control', "NO-CACHE")
        res.send(data);
        console.log(data.length);

    });
    client.write('{"command":"refill"}');
});

app.get('/cardgame/push/:dir', function(req,res){

    console.log("Got push request");
    client.once('data', 	function (data) {
        res.setHeader('Cache-Control', "NO-CACHE")
        res.send(data);
        console.log(data.length);
    });
    console.log('{"command":"'+req.params.dir+'"}');
    client.write('{"command":"'+req.params.dir+'"}');
});




app.get('/singlegame/start', function(req,res){
    //if (client.)

    console.log("Got start request");
    client.once('data', 	function (data) {

        res.setHeader('Cache-Control', "NO-CACHE")
        res.send(data);
        console.log(data.length);

    });
    var game = { host:"single", playerone:config.playerone, playertwo:config.playertwo};
    console.log(game);
    client.write(JSON.stringify(game));
});

app.get('/singlegame/turn', function(req,res){
    //if (client.)

    console.log("Got turn request");
    client.once('data', 	function (data) {

        res.setHeader('Cache-Control', "NO-CACHE")
        res.send(data);
        console.log(data.length);

    });
    client.write('{"command":"step"}');
});


app.get('/tournament', function(req,res){
    //if (client.)

    console.log("Got start request");

    client.on('data', 	function (data) {
        //var obj = JSON.parse(data);
        console.log(data.length);
        if (eventsocket) eventsocket.emit('state', data.toString());


    });
    var game = { host:"tournament", heat:15};
    console.log(game);
    client.write(JSON.stringify(game));
});