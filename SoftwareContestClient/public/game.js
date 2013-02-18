function reqListener () {
    input = true;

    var data = JSON.parse(this.responseText);
    var prow = 0;
    var pcol = 0;




    for(var row = 0; row<5;row++) {
        for(var col=0; col<5;col++) {
            var s = document.getElementById("tile"+row+col);
            if (data.card[row][col] == "R") s.innerHTML = '<img src="images/tile_repair.png">';
            if (data.card[row][col] == "S") s.innerHTML = '<img src="images/tile_siege.png">';
            if (data.card[row][col] == "A") s.innerHTML = '<img src="images/tile_army.png">';
            if (data.card[row][col] == "P") {
                prow = row; pcol = col;
                s.innerHTML = '<img src="images/tile_push_small.png">';
            }
            if (data.card[row][col] == "X") s.innerHTML = '';
        }

    }

    if (data.count) {
        impact+=data.count*multiplier;
        multiplier++;
    }
    document.getElementById('impact').innerHTML = '<p>'+impact+'</p>';

    var s = document.getElementById('pushtype');
    if (data.pushType == "R") s.innerHTML = '<img src="images/tile_repair.png">';
    if (data.pushType == "S") s.innerHTML = '<img src="images/tile_siege.png">';
    if (data.pushType == "A") s.innerHTML = '<img src="images/tile_army.png">';
    if (data.pushType == "X") s.innerHTML = '';

    if (showGuide) {
        document.getElementById('up').className = "tile tileRow"+prow+" tileCol"+pcol;
        document.getElementById('down').className = "tile tileRow"+prow+" tileCol"+pcol;
        document.getElementById('left').className = "tile tileRow"+prow+" tileCol"+pcol;
        document.getElementById('right').className = "tile tileRow"+prow+" tileCol"+pcol;

        document.getElementById('up').style.display =   (data.canPush.indexOf('U') != -1) ? 'block':'none';
        document.getElementById('down').style.display =   (data.canPush.indexOf('D') != -1) ? 'block':'none';
        document.getElementById('left').style.display =   (data.canPush.indexOf('L') != -1) ? 'block':'none';
        document.getElementById('right').style.display =   (data.canPush.indexOf('R') != -1) ? 'block':'none';
    } else   {
        document.getElementById('up').style.display =  'none';
        document.getElementById('down').style.display =  'none';
        document.getElementById('left').style.display =  'none';
        document.getElementById('right').style.display = 'none';
    }

    console.log(this.responseText);

}
function send(s) {
    xmlhttp.open("get", s, true);
    xmlhttp.send();
    input = false;
}
function onKeyDown(ev) {
    if (ev.keyCode == 83) {
        // Start game
        impact = 0;
        multiplier = 1;
        send(gameType+"/start");

    } else if (ev.keyCode == 82) {
        // Start game
        impact = 0;
        multiplier = 1;
        send("cardgame/refill");

    }else if (ev.keyCode == 72) {
        //Hide / Show Guides
        showGuide = !showGuide;

    } else if (ev.keyCode == 37) {
        // Left
        send("cardgame/push/L");
    } else if (ev.keyCode == 38) {
        // Up
        send("cardgame/push/U");
    } else if (ev.keyCode == 39) {
        // right
        send("cardgame/push/R");
    } else if (ev.keyCode == 40) {
        // Down
        send("cardgame/push/D");
    }
}
