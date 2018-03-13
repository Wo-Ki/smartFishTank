//Light_accessory.js

var PythonShell = require('python-shell');
// HomeKit types required
var types = require("../accessories/types.js");
var exports = module.exports = {};

var execute = function (accessory, characteristic, value) {
    console.log("executed accessory: " + accessory + ", and characteristic: " + characteristic + ", with value: " + value + ".");
};

exports.accessory = {
    displayName: "RGB LED",
    username: "1A:5B:3C:4A:5E:FF",
    pincode: "031-45-154",
    services: [{
        sType: types.ACCESSORY_INFORMATION_STYPE,
        characteristics: [{
            cType: types.NAME_CTYPE,
            onUpdate: null,
            perms: ["pr"],
            format: "string",
            initialValue: "RGB LED",
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Bla",
            designedMaxLength: 255
        }, {
            cType: types.MANUFACTURER_CTYPE,
            onUpdate: null,
            perms: ["pr"],
            format: "string",
            initialValue: "Oltica",
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Bla",
            designedMaxLength: 255
        }, {
            cType: types.MODEL_CTYPE,
            onUpdate: null,
            perms: ["pr"],
            format: "string",
            initialValue: "Rev-1",
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Bla",
            designedMaxLength: 255
        }, {
            cType: types.SERIAL_NUMBER_CTYPE,
            onUpdate: null,
            perms: ["pr"],
            format: "string",
            initialValue: "A1S2NASF88EW",
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Bla",
            designedMaxLength: 255
        }, {
            cType: types.IDENTIFY_CTYPE,
            onUpdate: null,
            perms: ["pw"],
            format: "bool",
            initialValue: false,
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Identify Accessory",
            designedMaxLength: 1
        }]
    }, {
        sType: types.LIGHTBULB_STYPE,
        characteristics: [{
            cType: types.NAME_CTYPE,
            onUpdate: null,
            perms: ["pr"],
            format: "string",
            initialValue: "RGB LED",
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Bla",
            designedMaxLength: 255
        }, {
            cType: types.POWER_STATE_CTYPE,
            onUpdate: function (value) {
                console.log("Change:", value);
                if (value) {
                    pyshell.send("{'M':'RL','N':'P','V':1}\n");
                } else {
                    pyshell.send("{'M':'RL','N':'P','V':0}\n");
                }
            },
            perms: ["pw", "pr", "ev"],
            format: "bool",
            initialValue: false,
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Turn On the Light",
            designedMaxLength: 1
        }, {
            cType: types.HUE_CTYPE,  //色度
            onUpdate: function (value) {
                console.log("Change:", value);
                pyshell.send("{'M':'RL','N':'H','V':"+String(value)+"}\n");
                execute("Test Accessory 1", "Light - Hue", value);
            },
            perms: ["pw", "pr", "ev"],
            format: "int",
            initialValue: 0,
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Doesn’t actually adjust Hue of Light",
            designedMinValue: 0,
            designedMaxValue: 360,
            designedMinStep: 1,
            unit: "arcdegrees"
        }, {
            cType: types.BRIGHTNESS_CTYPE,   //亮度
            onUpdate: function (value) {
                console.log("Change:", value);
                execute("Test Accessory 1", "Light - Brightness", value);
                pyshell.send("{'M':'RL','N':'V','V':"+String(value)+"}\n");
            },
            perms: ["pw", "pr", "ev"],
            format: "int",
            initialValue: 0,
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Doesn’t actually adjust Brightness of Light",
            designedMinValue: 0,
            designedMaxValue: 100,
            designedMinStep: 1,
            unit: "%"
        }, {
            cType: types.SATURATION_CTYPE,  //饱和
            onUpdate: function (value) {
                console.log("Change:", value);
                execute("Test Accessory 1", "Light - Saturation", value);
                pyshell.send("{'M':'RL','N':'S','V':"+String(value)+"}\n");
            },
            perms: ["pw", "pr", "ev"],
            format: "int",
            initialValue: 0,
            supportEvents: false,
            supportBonjour: false,
            manfDescription: "Doesn’t actually adjust Saturation of Light",
            designedMinValue: 0,
            designedMaxValue: 100,
            designedMinStep: 1,
            unit: "%"
        }]
    }]
}