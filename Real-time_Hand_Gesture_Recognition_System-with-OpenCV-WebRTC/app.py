from streamlit_webrtc import webrtc_streamer, RTCConfiguration
import av
import cv2
import mediapipe as mp
import time

# Setup MediaPipe for hand tracking
mpHands = mp.solutions.hands
hands = mpHands.Hands()
mpDraw = mp.solutions.drawing_utils

# Load face cascade classifier for face detection
cascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

# Video processing class
class VideoProcessor:
    def __init__(self):
        self.pTime = 0

    def recv(self, frame):
        frm = frame.to_ndarray(format="bgr24")

        # Perform face detection
        faces = cascade.detectMultiScale(cv2.cvtColor(frm, cv2.COLOR_BGR2GRAY), 1.1, 3)
        for x, y, w, h in faces:
            cv2.rectangle(frm, (x, y), (x+w, y+h), (0, 255, 0), 3)

        # Perform hand tracking
        imgRGB = cv2.cvtColor(frm, cv2.COLOR_BGR2RGB)
        results = hands.process(imgRGB)
        if results.multi_hand_landmarks:
            for handLms in results.multi_hand_landmarks:
                for lm in handLms.landmark:
                    h, w, c = frm.shape
                    cx, cy = int(lm.x*w), int(lm.y*h)
                    cv2.circle(frm, (cx, cy), 10, (0, 255, 0), cv2.FILLED)
                mpDraw.draw_landmarks(frm, handLms, mpHands.HAND_CONNECTIONS)

        # Calculate FPS
        cTime = time.time()
        fps = 1 / (cTime - self.pTime)
        self.pTime = cTime

        # Display FPS
        cv2.putText(frm, str(int(fps)), (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 3, (0, 255, 0), 3)

        return av.VideoFrame.from_ndarray(frm, format='bgr24')

# Create RTC Configuration
rtc_config = RTCConfiguration({"iceServers": [{"urls": ["stun:stun.l.google.com:19302"]}]})


# WebRTC Streamer
webrtc_streamer(key="example", video_processor_factory=VideoProcessor,
                rtc_configuration=rtc_config)
