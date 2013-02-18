var net = require('net');

var otherPlayer = 0;
var iAmPlayerInGame = 0;

if (process.send) {
    console.log("fork me, fork you");
    process.send("ready");
}

var player = net.createConnection(8124, "127.0.0.1", function () {
    // On Connection
    player.write(JSON.stringify({name:"lefty"}));

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
            console.log( "got a card"+data);
            console.log( "Left looks good");
            player.write(JSON.stringify({push:"L"}));
        } else if (obj.winner) {
            // got game state
            console.log( "got game state"+data);
            player.write(JSON.stringify({ready:true}));
        } else {
            console.log(data);
            console.log(obj);
            console.log(data.toString());
        }

    });
});



