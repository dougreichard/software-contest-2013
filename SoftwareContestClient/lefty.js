var net = require('net');
var otherPlayer = 0;
var iAmPlayerInGame = 0;

var name = 'lefty';
var pushes  = 'L';

if (process.argv.length>2) name = process.argv[2];
if (process.argv.length>3) pushes = process.argv[3];



if (process.send) {
    console.log("fork me, fork you");
    process.send("ready");
}

var player = net.createConnection(8124, "127.0.0.1", function () {
    // On Connection
    player.write(JSON.stringify({"name":name}));

    player.on("data",function (data) {
        var obj = JSON.parse(data);
        if (obj.id) {
            // GameStart
            otherPlayer = obj.id;
            iAmPlayerInGame = obj.player;
            console.log("I am playing "+obj.id+" and I am player "+obj.player);

            player.write(JSON.stringify({ready:true}));

        } else if (obj.card) {
            // got a card
            console.log( "got a card");
            console.log( "This looks good "+pushes);
            player.write(JSON.stringify({push:pushes}));
        } else if (obj.castles) {
            // got game state
            console.log( "got game state");
            player.write(JSON.stringify({ready:true}));
        } else {
            console.log(data);
            console.log(obj);
            console.log(data.toString());
        }

    });
});



