var net = require('net');

var otherPlayer = 0;
var iAmPlayerInGame = 0;


if (process.send) {
    console.log("Righteous fork");
    process.send("ready");
}


var player = net.createConnection(8124, "127.0.0.1", function () {
    // On Connection
    player.write(JSON.stringify({name:"righty"}));

    player.on("data",function (data) {
        var obj = JSON.parse(data);
        if (obj.id) {
            // GameStart
            otherPlayer = obj.id;
            iAmPlayerInGame = obj.player;
            console.log("I am playing "+obj.id+" and I am player "+obj.player);

            player.write(JSON.stringify({ready:true}));

        } else if (obj.card) {
            // Got card
            console.log( "got a card"+data);
            console.log( "Right looks good");
            player.write(JSON.stringify({push:"R"}));
        } else if (obj.castles) {
            // Game State
            console.log( "got game state"+data);
            player.write(JSON.stringify({ready:true}));
        } else {
            console.log(data);
            console.log(obj);
            console.log(data.toString());
        }

    });
});



