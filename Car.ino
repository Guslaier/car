#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
DNSServer dnsServer;
const byte DNS_PORT = 53;


const char *apSSID = "Car001";
WebServer server(80);


// กำหนด GPIO สำหรับมอเตอร์ (ผ่าน L298N หรือ H-Bridge)
const int IN1 = 26;  // ซ้ายเดินหน้า
const int IN2 = 25;  // ซ้ายถอยหลัง
const int IN3 = 33;  // ขวาเดินหน้า
const int IN4 = 32;  // ขวาถอยหลัง

void stopMotors() {
  Serial.println("stopMotors");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  Serial.println("moveForward");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  Serial.println("moveBackward");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  Serial.println("turnLeft");
    digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

}

void turnRight() {
  Serial.println("turnRight");
    digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void handleControl() {
  String dir = server.arg("dir");

  if (dir == "forward") moveForward();
  else if (dir == "backward") moveBackward();
  else if (dir == "left") turnLeft();
  else if (dir == "right") turnRight();
  else stopMotors();

  server.send(200, "text/plain", "OK");
}

void handleRoot() {
  String html = R"rawliteral(

<!DOCTYPE html>
<html lang="th">

<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Controler Car </title>
    <style>
        body {
            background: linear-gradient(135deg, #2eeded, #764ba2);
            color: #fff;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
            user-select: none;
        }

        h1 {
            margin-bottom: 40px;
            font-weight: 700;
            font-size: 2.5rem;
            text-shadow: 0 0 10px rgba(0, 0, 0, 0.3);
        }

        .controller {
            display: inline-grid;
            grid-template-columns: 180px 180px 180px;
            grid-template-rows: 180px 180px 180px;
            gap: 25px;
            justify-content: center;
            align-items: center;
        }

        button.btn {
            background: #8381e3;
            border: none;
            border-radius: 40px;
            box-shadow: 0 8px 16px rgba(90, 86, 249, 0.6);
            color: white;
            font-size: 28px;
            font-weight: 700;
            cursor: pointer;
            user-select: none;
            transition: background 0.3s, box-shadow 0.3s, transform 0.1s;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 12px;
            padding: 0;
            position: relative;
            width: 160px;
            height: 160px;
        }

        button.btn:active {
            transform: scale(0.95);
            box-shadow: 0 4px 8px rgba(222, 86, 249, 0.8);
            background: #4a44d7;
        }

        button.btn:hover {
            background: #7a76ff;
        }

        .btn-forward {
            grid-column: 2;
            grid-row: 1;
        }

        .btn-left {
            grid-column: 1;
            grid-row: 2;
        }

        .btn-stop {
            grid-column: 2;
            grid-row: 2;
            background: #d9534f;
            box-shadow: 0 8px 16px rgba(217, 83, 79, 0.6);
        }

        .btn-stop:hover {
            background: #c9302c;
        }

        .btn-backward {
            grid-column: 2;
            grid-row: 3;
        }

        .btn-right {
            grid-column: 3;
            grid-row: 2;
        }

        svg {
            width: 36px;
            height: 36px;
            fill: white;
            pointer-events: none;
        }

        @media (max-width: 600px) {
            .controller {
                grid-template-columns: 100px 100px 100px;
                grid-template-rows: 100px 100px 100px;
            }

            button.btn {
                font-size: 22px;
                width: 100px;
                height: 100px;

            }

            svg {
                width: 28px;
                height: 28px;
            }

            h1 {
                font-size: 1.8rem;
                margin-bottom: 30px;
            }
        }
    </style>
    <script>
        function send(dir) {
            fetch("/control?dir=" + dir).catch(e => {
                console.log("Error sending command:", e);
            });
        }

        function setupButton(id, direction) {
            const btn = document.getElementById(id);

            // Mouse events
            btn.addEventListener('mousedown', () => send(direction));
            btn.addEventListener('mouseup', () => send('stop'));
            btn.addEventListener('mouseleave', () => send('stop'));

            // Touch events
            btn.addEventListener('touchstart', (e) => {
                e.preventDefault(); // ป้องกันเลื่อนหน้า
                send(direction);
            }, { passive: false });

            btn.addEventListener('touchend', () => send('stop'));
            btn.addEventListener('touchcancel', () => send('stop'));
        }

        document.addEventListener("DOMContentLoaded", function () {
            setupButton('btn-forward', 'forward');
            setupButton('btn-backward', 'backward');
            setupButton('btn-left', 'left');
            setupButton('btn-right', 'right');
            setupButton('btn-stop', 'stop');
        });
    </script>
</head>

<body>
    <h1>ควบคุมรถ</h1>

    <div class="controller">
        <button id="btn-forward" class="btn btn-forward" aria-label="เดินหน้า">
            <svg viewBox="0 0 24 24" aria-hidden="true">
                <path d="M12 2l-8 8h6v8h4v-8h6z" />
            </svg>
        </button>

        <button id="btn-left" class="btn btn-left" aria-label="เลี้ยวซ้าย">
            <svg viewBox="0 0 24 24" aria-hidden="true">
                <path d="M2 12l8-8v6h8v4h-8v6z" />
            </svg>
        </button>

        <button id="btn-stop" class="btn btn-stop" aria-label="หยุด">
            <svg viewBox="0 0 100 100" aria-hidden="true">
                <text x="50%" y="55%" text-anchor="middle" dominant-baseline="middle" font-size="30" fill="#fff">STOP</text>
            </svg>
        </button>

        <button id="btn-right" class="btn btn-right" aria-label="เลี้ยวขวา">
            <svg viewBox="0 0 24 24" aria-hidden="true">
                <path d="M22 12l-8 8v-6h-8v-4h8v-6z" />
            </svg>
        </button>

        <button id="btn-backward" class="btn btn-backward" aria-label="ถอยหลัง">
            <svg viewBox="0 0 24 24" aria-hidden="true">
                <path d="M12 22l8-8h-6v-8h-4v8h-6z" />
            </svg>
        </button>
    </div>
</body>

</html>


  )rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  IPAddress apIP(192, 168, 4, 1);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  dnsServer.start(DNS_PORT, "bot.car", apIP);
  Serial.println("Wi-Fi พร้อม: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/control", handleControl);
  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}
