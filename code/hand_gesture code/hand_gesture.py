import cv2
import time
import numpy as np
import HandTrackingModule as htm
import math
import requests
################################
wCam, hCam = 1700, 1500
################################

cap = cv2.VideoCapture(0)
cap.set(3, wCam)
cap.set(4, hCam)
pTime = 0

detector = htm.handDetector(detectionCon=0.7)

fist = False
x1, y1 = 0, 0
x2, y2 = 0, 0
cx, cy = 0, 0
minGrip = 0
maxGrip = 100
gripBar = 0
gripPer = 0
top = False
bottom = False
left = False
right = False
length = 0
grip = 0

last_grip = -1

interval = 0.2  # seconds
last_called = time.time()


def toggle_checkbox(inputT, inputI, inputA, inputB):
    url = "http://192.168.4.1/cmd"
    params = {
        "inputT": inputT,
        "inputI": inputI,
        "inputA": inputA,
        "inputB": inputB
    }
    try:
        response = requests.get(
            url, params=params, headers={"Connection": "close"}, timeout=1
        )
        if response.ok:
            print("✅ Request successful:")
        else:
            print("⚠️ Request failed:", response.status_code)
    except requests.exceptions.RequestException as e:
        print("❌ Connection error:", e)
  



while True:
    success, img = cap.read()
    img = detector.findHands(img)
    lmList = detector.findPosition(img, draw=False)

    if len(lmList) != 0 and len(lmList[0]) != 0:
        fingers = detector.fingersUp()

        # Detect fist
        if fingers == [0, 0, 0, 0, 0]:
            fist = True
        else:
            fist = False    

    if len(lmList) != 0 :
        # Thumb tip
        x1, y1 = lmList[4][1], lmList[4][2]
        # Index tip
        x2, y2 = lmList[8][1], lmList[8][2]

        # Midpoint
        cx, cy = (x1 + x2) // 2, (y1 + y2) // 2

        # Draw markers
        cv2.circle(img, (x1, y1), 10, (255, 0, 255), cv2.FILLED)
        cv2.circle(img, (x2, y2), 10, (255, 0, 255), cv2.FILLED)
        cv2.line(img, (x1, y1), (x2, y2), (255, 0, 255), 3)
        cv2.circle(img, (cx, cy), 10, (255, 0, 255), cv2.FILLED)

        # Distance between thumb and index
        length = math.hypot(x2 - x1, y2 - y1)
        # print(length)

        # Interpolate length into Gripper and bar position
        grip = np.interp(length, [40, 150], [minGrip, maxGrip])
        gripBar = np.interp(length, [40, 150], [400, 150])
        gripPer = np.interp(length, [40, 150], [0, 100])

        # # Draw bar
        # cv2.rectangle(img, (50, 150), (85, 400), (255, 0, 0), 3)
        # cv2.rectangle(img, (50, int(gripBar)), (85, 400), (255, 0, 0), cv2.FILLED)
        # cv2.putText(img, f'{int(gripPer)} %', (40, 450), cv2.FONT_HERSHEY_COMPLEX,
        #             1, (255, 0, 0), 3)

        # Draw circle when very close
        if length < 40:
            cv2.circle(img, (cx, cy), 10, (0, 255, 0), cv2.FILLED)
        grip = int(grip)
        print(f"Length: {int(length)}, Gripper: {grip}")

    # Get thumb coordinates for direction checks
    if len(lmList) != 0 and fist:
        x, y = lmList[4][1], lmList[4][2]
        h, w, _ = img.shape  
        x = w - x  # Flip for correct orientation

        top, bottom, left, right = False, False, False, False
        if y < h // 3: top = True
        elif y > 2 * h // 3: bottom = True
        elif x < w // 3: left = True
        elif x > 2 * w // 3: right = True

        print(f"Top: {top}, Bottom: {bottom}, Left: {left}, Right: {right}")

    # FPS
    cTime = time.time()
    fps = 1 / (cTime - pTime)
    pTime = cTime

    

    img = cv2.flip(img, 1)

    cv2.putText(img, f'FPS: {int(fps)}', (40, 50), cv2.FONT_HERSHEY_COMPLEX,
                1, (255, 0, 0), 3)
    # Draw bar
    cv2.rectangle(img, (50, 150), (85, 400), (255, 0, 0), 3)
    cv2.rectangle(img, (50, int(gripBar)), (85, 400), (255, 0, 0), cv2.FILLED)
    cv2.putText(img, f'{int(gripPer)} %', (40, 450), cv2.FONT_HERSHEY_COMPLEX,
                1, (255, 0, 0), 3)
    
    # # # # For sending request to server # # # #  # #
    # ---- For sending request to server ----
    if time.time() - last_called >= interval:
        last_called = time.time()

        if top:
            toggle_checkbox(1,24,0,0)
        elif bottom:
            toggle_checkbox(1,25,0,0)
        elif right:
            toggle_checkbox(1,26,0,0)
        elif left:
            toggle_checkbox(1,27,0,0)

        # Send grip only if nonzero and changed
        if grip != 0 and grip != last_grip:
            if grip<15 :
                grip = 0
            toggle_checkbox(28, int(grip), 0, 0)
            last_grip = grip
    

    
    
    






    # # # # # # # # Design of camera feedback below


    # # Draw 3x3 grid
    # h, w, _ = img.shape
    # cv2.line(img, (w // 3, 0), (w // 3, h), (250, 250, 250), 2)
    # cv2.line(img, (2 * w // 3, 0), (2 * w // 3, h), (0, 255, 0), 2)
    # cv2.line(img, (0, h // 3), (w, h // 3), (0, 255, 0), 2)
    # cv2.line(img, (0, 2 * h // 3), (w, 2 * h // 3), (0, 255, 0), 2)

    # Draw 3x3 grid with transparency (black/grey lines)
    overlay = img.copy()
    h, w, _ = img.shape

    line_color = (50, 50, 50)  # dark grey (use (0,0,0) for pure black)

    # Vertical lines
    cv2.line(overlay, (w // 3, 0), (w // 3, h), line_color, 2)
    cv2.line(overlay, (2 * w // 3, 0), (2 * w // 3, h), line_color, 2)
    # Horizontal lines
    cv2.line(overlay, (0, h // 3), (w, h // 3), line_color, 2)
    cv2.line(overlay, (0, 2 * h // 3), (w, 2 * h // 3), line_color, 2)

    # Blend with transparency
    alpha = 0.4  # 0 = fully transparent, 1 = fully solid
    img = cv2.addWeighted(overlay, alpha, img, 1 - alpha, 0)


    #
    
              
    cv2.imshow("Img", img)
    cv2.waitKey(1)
