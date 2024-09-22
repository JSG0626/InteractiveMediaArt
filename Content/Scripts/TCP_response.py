import cv2
import mediapipe as mp
import socket
import json
import struct
import time

# MediaPipe Pose 설정
mp_pose = mp.solutions.pose
mp_drawing = mp.solutions.drawing_utils  # 랜드마크와 연결선을 그리기 위해 사용
pose = mp_pose.Pose()

# 랜드마크 이름 정의
LANDMARK_NAMES = {
    0: "NOSE",
    11: "LEFT_SHOULDER",
    12: "RIGHT_SHOULDER",
    13: "LEFT_ELBOW",
    14: "RIGHT_ELBOW",
    15: "LEFT_WRIST",
    16: "RIGHT_WRIST",
    17: "LEFT_PINKY",
    18: "RIGHT_PINKY",
    19: "LEFT_INDEX",
    20: "RIGHT_INDEX",
    21: "LEFT_THUMB",
    22: "RIGHT_THUMB",
    23: "LEFT_HIP",
    24: "RIGHT_HIP",
    25: "LEFT_KNEE",
    26: "RIGHT_KNEE",
    27: "LEFT_ANKLE",
    28: "RIGHT_ANKLE",
    29: "LEFT_HEEL",
    30: "RIGHT_HEEL",
    31: "LEFT_FOOT_INDEX",
    32: "RIGHT_FOOT_INDEX"
}

# 제외할 랜드마크 인덱스 (1~10번 제외)
EXCLUDED_LANDMARKS = set(range(1, 11))

# 소켓 설정
server_ip = socket.gethostbyname(socket.gethostname())
server_port = 8090

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((server_ip, server_port))
server_socket.listen(3)  # 클라이언트 연결을 대기 (동시 최대 3명)

print(f"TCP 서버가 {server_ip}:{server_port}에서 대기 중입니다...")

# 클라이언트 연결 대기
conn, addr = server_socket.accept()
print(f"{addr}와 연결되었습니다.")

# 웹캠 초기화
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Error: Could not open video device.")
    exit()

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break


    # 이미지 전처리
    image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    
    # 포즈 감지
    pose_results = pose.process(image)

    if pose_results.pose_landmarks:
        pose_landmarks = {}
        for idx, landmark in enumerate(pose_results.pose_landmarks.landmark):
            if idx == 0 or idx not in EXCLUDED_LANDMARKS:  # 0번 포함, 1~10번 제외
                landmark_name = LANDMARK_NAMES.get(idx, f"landmark_{idx}")
                x, y = landmark.x, landmark.y
                pose_landmarks[landmark_name] = {'x': x, 'y': y}
        
        # ------------------------------------------------------------------------------------
        # JSON으로 좌표 데이터 변환
        data = json.dumps(pose_landmarks)
        
        # 데이터 길이를 계산하고 4바이트 빅 엔디안 형식으로 패킹하여 전송
        data_length = len(data)
        packed_length = struct.pack('!I', data_length)  # 4바이트 빅 엔디안으로 데이터 길이 전송
        
        try:
            # 데이터 길이 전송
            conn.sendall(packed_length)
            
            # JSON 데이터 전송
            conn.sendall(data.encode('utf-8'))
            print(f'Sent data length: {data_length}, Sent JSON data: {data}')
        
        except socket.error as e:
            print(f"Failed to send data: {e}")
        # ------------------------------------------------------------------------------------
        
        # 스켈레톤 그리기 (전체 랜드마크를 그립니다)
        mp_drawing.draw_landmarks(
            frame, 
            pose_results.pose_landmarks, 
            mp_pose.POSE_CONNECTIONS)

    # 영상 좌우 반전 및 출력
    frame = cv2.flip(frame, 1)
    cv2.imshow('MediaPipe Pose', frame)

    if cv2.waitKey(10) & 0xFF == ord('q'):
        break
    #time.sleep(0.1)

# 리소스 정리
cap.release()
cv2.destroyAllWindows()
conn.close()
server_socket.close()
