#!/usr/bin/env node

var uartmode = require("./app.js");

var operation = process.argv[2];
var port = process.argv[3];
var mode = process.argv[4];

if(operation === "get" && port !== undefined){
    var currentMode = uartmode.getUartMode(process.argv[3]);
    console.log("Mode is: "+currentMode);
}else if(operation === "set" && port !== undefined && mode !== undefined){
    var result = uartmode.setUartMode(port, mode);
    if(result === true){
        console.log("Mode on port "+port+" successfully set to: "+mode);
    }else{
        console.log("Unable to set mode on port "+port);
    }
}else{
    console.log("Usage: test get <port>\nor: test set <port> <mode>\nwhere <port> is e.g. 'dev/ttyO3' and <mode> is 485, 422 or 232")
}
