const uartmode = require("./build/Release/tibbo-uartmode");

function modeToInteger(mode) {
    switch (mode) {
        case uartmode.CuartMode232: return "232";
        case uartmode.CuartMode422: return "422";
        case uartmode.CuartMode485: return "485";
        default: return null;
    }
}

function integerToMode(modeRaw) {
    var mode = parseInt(modeRaw);
    switch (mode) {
        case 232: return uartmode.CuartMode232;
        case 422: return uartmode.CuartMode422;
        case 485: return uartmode.CuartMode485;
        default: return null;
    }
}

exports.setUartMode = function(port, modeRaw){
    var mode = integerToMode(modeRaw);
    if(mode !== null){
        uartmode.tibbo_uartmode.S(port, mode);
        return true;
    }else{
        return false;
    }
};

exports.getUartMode = function(port){
    var currentMode =  uartmode.tibbo_uartmode.G(port);
    return modeToInteger(currentMode);
};