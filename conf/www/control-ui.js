var currentLocation = window.location;
var ws = null;
var lastMachineState = "off";
const colorActive = "#333333";
const colorInactive = "#cccccc";
const colorLightOn = "#aaff22";
const colorLightOff = colorInactive;
const colorRed = "#ff0000";

// Handle new machine state
function receiveMachineState(js) {
    console.log("state: " + js.state);
    console.log("feeder-speed: " + js.feeder_speed);
    console.log("pre-heater-temperature: " + js.pre_heater_temperature);
    console.log("merger-heater-temperature: " + js.merger_heater_temperature);
    var controlUi = document.querySelector('#control-ui').getSVGDocument();
    lastMachineState = js.state;
    controlUi.getElementById("label-feeder-speed").textContent = js.feeder_speed;
    controlUi.getElementById("label-pre-heater-temperature").textContent = js.pre_heater_temperature;
    controlUi.getElementById("label-merger-heater-temperature").textContent = js.merger_heater_temperature;
    updateUserControl();
}

// Request current machine state
function requestMachineState() {
    console.log("requesting current machine state...");
    ws.send(
        JSON.stringify({
            type: "request-state",
        })
    );
}

function sendMachineCommand(commandName) {
    console.log("sending machine command '" + commandName + "'");
    ws.send(
        JSON.stringify({
            type: "request-command",
            command: commandName,
        })
    );
}

// Connect to websocket server
function connect() {
    ws = new WebSocket("ws://" + currentLocation.host + "/websocket");
    ws.onopen = (event) => {
        console.log("...opened connection");
        requestMachineState();
    };

    ws.onclose = (event) => {
        console.log("closing connection...");
        setTimeout(function () {
            connect();
        }, 500);
    };

    ws.onmessage = (event) => {
        console.log("received message: '" + event.data + "'");
        var js = JSON.parse(event.data);
        if (js != null && "type" in js) {
            if (
                js.type == "response-state" ||
                js.type == "notification-state"
            ) {
                receiveMachineState(js);
            } else if (js.type == "response-command") {
                if (js.result == "error") {
                    alert("Command failed");
                    console.log("command response: " + js.result);
                } else {
                    requestMachineState();
                }
            }
        }
    };

    ws.onerror = (error) => {
        console.log(error);
        ws.close();
        ws = null;
    };
}

function changeButtonColor(button, active) {
    color = active ? colorActive : colorInactive;
    if (active) {
        button.onclick = function () {
            sendMachineCommand(button.id.replace("button-", ""));
        };
    }
    else {
        button.onclick = null
    }
    for (child of button.children) {
        if (!child.classList.contains("svg-button")) {
            child.style.fill = color
        }
        child.style.stroke = color
    }
}

// Update UI
function updateUserControl() {
    var controlUi = document.querySelector('#control-ui').getSVGDocument();
    var buttonStart = controlUi.getElementById("button-start");
    var buttonStartMaintenance = controlUi.getElementById(
        "button-start-maintenance"
    );
    var buttonStop = controlUi.getElementById("button-stop");
    var buttonDecFeederSpeed = controlUi.getElementById(
        "button-decrease-feeder-speed"
    );
    var buttonIncFeederSpeed = controlUi.getElementById(
        "button-increase-feeder-speed"
    );
    var buttonIncPreHeaterTemperature = controlUi.getElementById(
        "button-increase-pre-heater-temperature"
    );
    var buttonDecPreHeaterTemperature = controlUi.getElementById(
        "button-decrease-pre-heater-temperature"
    );
    var buttonIncMergerHeaterTemperature = controlUi.getElementById(
        "button-increase-merger-heater-temperature"
    );
    var buttonDecMergerHeaterTemperature = controlUi.getElementById(
        "button-decrease-merger-heater-temperature"
    );
    var lightPower = controlUi.getElementById("light-power");
    var lightRunning = controlUi.getElementById("light-running");
    var lightError = controlUi.getElementById("light-error");
    lightError.style.fill = colorLightOff;
    console.log("Update: " + lastMachineState);
    if (lastMachineState == "off") {
        controlUi.getElementById("button-power").onclick = function () {
            sendMachineCommand("switch-on");
        };
        changeButtonColor(buttonStart, false);
        changeButtonColor(buttonStartMaintenance, false);
        changeButtonColor(buttonStop, false);
        changeButtonColor(buttonDecFeederSpeed, false);
        changeButtonColor(buttonIncFeederSpeed, false);
        changeButtonColor(buttonDecPreHeaterTemperature, false);
        changeButtonColor(buttonIncPreHeaterTemperature, false);
        changeButtonColor(buttonDecMergerHeaterTemperature, false);
        changeButtonColor(buttonIncMergerHeaterTemperature, false);
        lightPower.style.fill = colorLightOff;
        lightRunning.style.fill = colorLightOff;
    } else if (lastMachineState == "stopped") {
        controlUi.getElementById("button-power").onclick = function () {
            sendMachineCommand("switch-off");
        };
        changeButtonColor(buttonStart, true);
        changeButtonColor(buttonStartMaintenance, true);
        changeButtonColor(buttonStop, false);
        changeButtonColor(buttonDecFeederSpeed, false);
        changeButtonColor(buttonIncFeederSpeed, false);
        changeButtonColor(buttonDecPreHeaterTemperature, false);
        changeButtonColor(buttonIncPreHeaterTemperature, false);
        changeButtonColor(buttonDecMergerHeaterTemperature, false);
        changeButtonColor(buttonIncMergerHeaterTemperature, false);
        lightPower.style.fill = colorLightOn;
        lightRunning.style.fill = colorLightOff;
    } else if (lastMachineState == "starting") {
        controlUi.getElementById("button-power").onclick = function () {
            sendMachineCommand("switch-off");
        };
        changeButtonColor(buttonStart, false);
        changeButtonColor(buttonStartMaintenance, false);
        changeButtonColor(buttonStop, true);
        changeButtonColor(buttonDecFeederSpeed, false);
        changeButtonColor(buttonIncFeederSpeed, false);
        changeButtonColor(buttonDecPreHeaterTemperature, false);
        changeButtonColor(buttonIncPreHeaterTemperature, false);
        changeButtonColor(buttonDecMergerHeaterTemperature, false);
        changeButtonColor(buttonIncMergerHeaterTemperature, false);
        lightPower.style.fill = colorLightOn;
        lightRunning.style.fill = colorLightOff;
    } else if (lastMachineState == "running") {
        controlUi.getElementById("button-power").onclick = function () {
            sendMachineCommand("switch-off");
        };
        changeButtonColor(buttonStart, false);
        changeButtonColor(buttonStartMaintenance, false);
        changeButtonColor(buttonStop, true);
        changeButtonColor(buttonDecFeederSpeed, true);
        changeButtonColor(buttonIncFeederSpeed, true);
        changeButtonColor(buttonDecPreHeaterTemperature, false);
        changeButtonColor(buttonIncPreHeaterTemperature, false);
        changeButtonColor(buttonDecMergerHeaterTemperature, false);
        changeButtonColor(buttonIncMergerHeaterTemperature, false);
        lightPower.style.fill = colorLightOn;
        lightRunning.style.fill = colorLightOn;
    } else if (lastMachineState == "running-maintenance") {
        controlUi.getElementById("button-power").onclick = function () {
            sendMachineCommand("switch-off");
        };
        changeButtonColor(buttonStart, false);
        changeButtonColor(buttonStartMaintenance, false);
        changeButtonColor(buttonStop, true);
        changeButtonColor(buttonDecFeederSpeed, true);
        changeButtonColor(buttonIncFeederSpeed, true);
        changeButtonColor(buttonDecPreHeaterTemperature, true);
        changeButtonColor(buttonIncPreHeaterTemperature, true);
        changeButtonColor(buttonDecMergerHeaterTemperature, true);
        changeButtonColor(buttonIncMergerHeaterTemperature, true);
        lightPower.style.fill = colorLightOn;
        lightRunning.style.fill = colorLightOn;
    }
    // updateEventHandlers();
}

document.addEventListener("DOMContentLoaded", function (event) {
    document.querySelector('#control-ui').addEventListener("load", function (event) {
        updateUserControl();
        connect();
    });
});
