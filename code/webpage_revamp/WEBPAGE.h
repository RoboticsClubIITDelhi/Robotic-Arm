const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>Servo Driver with ESP32</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
        font-family: Arial;
        display: inline-block;
        background: #000000;
        color: #efefef;
        text-align: center;
    }

    h2 {
        font-size: 3.0rem;
    }

    p {
        font-size: 1.0rem;
    }

    body {
        max-width: 600px;
        margin: 0px auto;
        padding-bottom: 25px;
    }

    button {
        display: inline-block;
        margin: 5px;
        padding: 10px 10px;
        border: 0;
        line-height: 21px;
        cursor: pointer;
        color: #fff;
        background: #4247b7;
        border-radius: 5px;
        font-size: 21px;
        outline: 0;
        width: 100px

        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;

        user-select: none;
    }

    button:hover {
        background: #ff494d
    }

    button:active {
        background: #f21c21
    }

    .dance-button {
        background: #ff6b35;
        width: 150px;
        font-weight: bold;
    }

    dance-button:hover {
        background: #ff8c42;
    }

    dance-button:active {
        background: #e55a2b;
    }
    input-field {
            margin: 5px;
            padding: 8px;
            border: 1px solid #ccc;
            border-radius: 4px;
            width: 80px;
            font-size: 14px;
    }
        .button-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 10px;
            margin: 20px 0;
        }
        
        .control-group {
            background-color: #2c3e50;
            padding: 15px;
            margin: 15px 0;
            border-radius: 8px;
            border: 2px solid #3498db;
        }
        
        .input-group {
            display: flex;
            align-items: center;
            margin: 10px 0;
            gap: 15px;
        }
        
        .input-group label {
            min-width: 80px;
            font-weight: bold;
            color: #3498db;
        }
        
        input[type="text"], input[type="number"] {
            padding: 8px;
            border: 2px solid #3498db;
            border-radius: 4px;
            background-color: #34495e;
            color: #ecf0f1;
            font-size: 14px;
            width: 100px;
        }
        
        input[type="text"]:focus, input[type="number"]:focus {
            outline: none;
            border-color: #2980b9;
            background-color: #2c3e50;
        }
        
        /* Slider Styles */
        .slider {
            -webkit-appearance: none;
            width: 200px;
            height: 8px;
            border-radius: 5px;
            background: #34495e;
            outline: none;
            margin: 0 10px;
        }
        
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #3498db;
            cursor: pointer;
            border: 2px solid #2980b9;
        }
        
        .slider::-moz-range-thumb {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #3498db;
            cursor: pointer;
            border: 2px solid #2980b9;
        }
        
        .slider:hover::-webkit-slider-thumb {
            background: #2980b9;
        }
        
        .slider:hover::-moz-range-thumb {
            background: #2980b9;
        }
        
        .slider-container {
            display: flex;
            flex-direction: column;
            gap: 5px;
        }
        
        .slider-row {
            display: flex;
            align-items: center;
            gap: 15px;
        }
        
        .value-display {
            font-size: 12px;
            color: #95a5a6;
        }


    </style>
</head>

<body>
    <h3>SERVO DRIVER with ESP32</h3>
    <p>
    <span id="IDValue">Click this button to start searching servos.</span>
    <p>
    <label align="center"><button class="button" onclick="toggleCheckbox(9, 0, 0, 0);">Start Searching</button></label>
    <p>
    <span id="STSValue">Single servo infomation.</span>
    <p>
        <label align="center"><button class="button" onclick="toggleCheckbox(0, 1, 0, 0);">ID Select+</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(0, -1, 0, 0);">ID Select-</button></label>
    <p>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 1, 0, 0);">Middle</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 2, 0, 0);">Stop</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 3, 0, 0);">Release</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 4, 0, 0);">Torque</button></label>
    <p>
        <label align="center"><button class="button" onmousedown="toggleCheckbox(1, 5, 0, 0);" ontouchstart="toggleCheckbox(1, 5, 0, 0);" onmouseup="toggleCheckbox(1, 2, 0, 0);" ontouchend="toggleCheckbox(1, 2, 0, 0);">Position+</button></label>
        <label align="center"><button class="button" onmousedown="toggleCheckbox(1, 6, 0, 0);" ontouchstart="toggleCheckbox(1, 6, 0, 0);" onmouseup="toggleCheckbox(1, 2, 0, 0);" ontouchend="toggleCheckbox(1, 2, 0, 0);">Position-</button></label>
    <p>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 7, 0, 0);">Speed+</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 8, 0, 0);">Speed-</button></label>
    <p>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 9, 0, 0);">ID to Set+</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 10, 0, 0);">ID to Set-</button></label>
    <p>
        <label align="center"><button class="button" onclick="setMiddle();">Set Middle Position</button></label>
        <label align="center"><button class="button" onclick="setNewID();">Set New ID</button></label>
    <p>
        <label align="center"><button class="button" onclick="setServoMode();">Set Servo Mode</button></label>
        <label align="center"><button class="button" onclick="setStepperMode();">Set Motor Mode</button></label>
    <p>
        <label align="center"><button class="button dance-button" onclick="startDance();">🕺 Dance</button></label>
    <p>
        <label align="center"><button class="button" id="serialForwarding" onclick="serialForwarding();">Start Serial Forwarding</button></label>
    <p>
        <label align="center"><button class="button" onclick="setRole(0);">Normal</button></label>
        <label align="center"><button class="button" onclick="setRole(1);">Leader</button></label>
        <label align="center"><button class="button" onclick="setRole(2);">Follower</button></label>
    <p>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 20, 0, 0);">RainbowON</button></label>
        <label align="center"><button class="button" onclick="toggleCheckbox(1, 21, 0, 0);">RainbowOFF</button></label>
    <p>
    </div>
        <div class="control-group">
            <h2>Inverse Kinematic Control</h2>
            
            <!-- R Parameter -->
            <div class="input-group">
                <label for="r_input">R:</label>
                <input type="number" id="r_input" value="50" step="1" min="50" max="350" 
                       onchange="updateSlider('r')" oninput="updateSlider('r')">
                <div class="slider-container">
                    <input type="range" id="r_slider" class="slider" min="50" max="350" value="50" step="1"
                           oninput="updateTextbox('r')">
                    <div class="value-display">Range: 50 to 350</div>
                </div>
            </div>
            
            <!-- Theta Parameter -->
            <div class="input-group">
                <label for="theta_input">θ:</label>
                <input type="number" id="theta_input" value="0" step="1" min="-90" max="90"
                       onchange="updateSlider('theta')" oninput="updateSlider('theta')">
                <div class="slider-container">
                    <input type="range" id="theta_slider" class="slider" min="-90" max="90" value="0" step="1"
                           oninput="updateTextbox('theta')">
                    <div class="value-display">Range: -90° to 90°</div>
                </div>
            </div>
            
            <!-- Z Parameter -->
            <div class="input-group">
                <label for="z_input">Z:</label>
                <input type="number" id="z_input" value="0" step="1" min="-100" max="300"
                       onchange="updateSlider('z')" oninput="updateSlider('z')">
                <div class="slider-container">
                    <input type="range" id="z_slider" class="slider" min="-100" max="300" value="0" step="1"
                           oninput="updateTextbox('z')">
                    <div class="value-display">Range: -100 to 300</div>
                </div>
            </div>
            <button onclick="sendIKCommand()" style="background-color: #FF6B35; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px;">
                Execute IK Movement

        </div>


    <script>
        serialForwardStatus = false;
        function updateTextbox(parameter) {
            const slider = document.getElementById(parameter + '_slider');
            const textbox = document.getElementById(parameter + '_input');
            textbox.value = slider.value;
            
            // Optional: trigger any existing inverse kinematics calculations
            if (typeof calculateInverseKinematics === 'function') {
                calculateInverseKinematics();
            }
        }
        
        function updateSlider(parameter) {
            const slider = document.getElementById(parameter + '_slider');
            const textbox = document.getElementById(parameter + '_input');
            const value = parseFloat(textbox.value);
            
            // Validate input within slider range
            const min = parseFloat(slider.min);
            const max = parseFloat(slider.max);
            
            if (!isNaN(value) && value >= min && value <= max) {
                slider.value = value;
            } else if (!isNaN(value)) {
                // Clamp to valid range
                const clampedValue = Math.max(min, Math.min(max, value));
                textbox.value = clampedValue;
                slider.value = clampedValue;
            }
            
            // Optional: trigger any existing inverse kinematics calculations
            if (typeof calculateInverseKinematics === 'function') {
                calculateInverseKinematics();
            }
        }


        function toggleCheckbox(inputT, inputI, inputA, inputB) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "cmd?inputT="+inputT+"&inputI="+inputI+"&inputA="+inputA+"&inputB="+inputB, true);
            xhr.send();
        }

        function ctrlMode() {
            xhr.open("GET", "ctrl", true);
            xhr.send();
        }

        function startDance(){
            toggleCheckbox(1, 22, 0, 0);
        }

        setInterval(function() {
          getData();
        }, 300);

        setInterval(function() {
          getServoID();
        }, 1500);

        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                  document.getElementById("STSValue").innerHTML =
                  this.responseText;
                }
            };
            xhttp.open("GET", "readSTS", true);
            xhttp.send();
        }

        function getServoID() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                  document.getElementById("IDValue").innerHTML =
                  this.responseText;
                }
            };
            xhttp.open("GET", "readID", true);
            xhttp.send();
        }

        function setRole(modeNum){
            if(modeNum == 0){
                var r=confirm("Set the role as Normal. Dev won't send or receive data via ESP-NOW.");
                if(r==true){
                    toggleCheckbox(1, 17, 0, 0);
                }
            }
            if(modeNum == 1){
                var r=confirm("Set the role as Leader. Dev will send data via ESP-NOW.");
                if(r==true){
                    toggleCheckbox(1, 18, 0, 0);
                }
            }
            if(modeNum == 2){
                var r=confirm("Set the role as Follower. Dev will receive data via ESP-NOW.");
                if(r==true){
                    toggleCheckbox(1, 19, 0, 0);
                }
            }
        }

        function setMiddle(){
            var r=confirm("The middle position of the active servo will be set.");
            if(r==true){
                toggleCheckbox(1, 11, 0, 0);
            }
        }

        function setServoMode(){
            var r=confirm("The active servo will be set as servoMode.");
            if(r==true){
                toggleCheckbox(1, 12, 0, 0);
            }
        }

        function setStepperMode(){
            var r=confirm("The active servo will be set as motorMode.");
            if(r==true){
                toggleCheckbox(1, 13, 0, 0);
            }
        }

        function setNewID(){
            var r=confirm("A new ID of the active servo will be set.");
            if(r==true){
                toggleCheckbox(1, 16, 0, 0);
            }
        }

        function serialForwarding(){
            if(!serialForwardStatus){
                var r=confirm("Do you want to start serial forwarding?");
                if(r){
                    toggleCheckbox(1, 14, 0, 0);
                    serialForwardStatus = true;
                    document.getElementById("serialForwarding").innerHTML = "Stop Serial Forwarding";
                }
            }
            else{
                var r=confirm("Do you want to stop serial forwarding?");
                if(r){
                    toggleCheckbox(1, 15, 0, 0);
                    serialForwardStatus = false;
                    document.getElementById("serialForwarding").innerHTML = "Start Serial Forwarding";
                }
            }
        }
        function sendIKCommand() {
          var r = document.getElementById('r_input').value;
          var theta = document.getElementById('theta_input').value;
          var z = document.getElementById('z_input').value;
    
          var xhttp = new XMLHttpRequest();
          xhttp.open("GET", "/cmd?T=23&I=" + r + "&A=" + theta + "&B=" + z, true);
          xhttp.send();
}


    </script>
</body>
</html>
)rawliteral";
