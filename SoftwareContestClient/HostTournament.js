var net = require('net'),
    fs = require('fs'),
    cp = require('child_process');

/*
 var file = __dirname + '/configgame.json';
 var config = { playerone:"lefty", playertwo:"righty"};
 fs.readFile(file, 'utf8', function (err, data) {
 if (err) {
 console.log('Error: ' + err);
 return;
 }

 config = JSON.parse(data);

 console.log(data);

 var l = cp.fork('lefty.js');
 l.on('message', function (msg){

 var r = cp.fork('righty.js');
 r.on('message', function (msg){
 runGame();
 });
 });
 });
 */


function runTournament() {
    var host = net.createConnection(8124, "127.0.0.1", function () {

        //host.write(JSON.stringify({host:"single", playerone:"lefty", playertwo:"righty"}));
        host.write(JSON.stringify({host:"tournament", "heat":10}));

        host.on("data",function (data) {
            var obj = JSON.parse(data);
            if (obj.castles) {
                if (obj.winner != 0) {
                    console.log( "got game state"+data);
                    //
                    if (obj.winner == 1) console.log("the winner in lefty")
                    else if (obj.winner == 2) console.log("the winner in righty")
                    process.exit();

                } else {
                    console.log( "got game state"+data);
                    host.write(JSON.stringify({command:"step"}));
                }
            } else {
                console.log(data);
                console.log(obj);
                console.log(data.toString());
            }

        });
    });
}

var players = [
    {name:"lefty", pushes:"L"},
    {name:"right", pushes:"R"},
    {name:"uppy", pushes:"U"},
    {name:"downy", pushes:"D"},
    {name:"fred", pushes:"L"},
    {name:"barney", pushes:"R"},
    {name:"betty", pushes:"U"},
    {name:"wilma", pushes:"D"}
];


var count = 0;
function loadPlayer() {
    count ++;
    if (count >= players.length) runTournament();
}


for(var i=0;i<players.length;i++) {
    var r = cp.fork('lefty.js', [players[i].name,players[i].pushes]);

}


setTimeout(runTournament, 3000);



