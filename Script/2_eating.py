import cv2
import mediapipe as mp
import socket
import threading
import time

# Mediapipe 솔루션 초기화
mp_pose = mp.solutions.pose
mp_hands = mp.solutions.hands
mp_face_mesh = mp.solutions.face_mesh
mp_drawing = mp.solutions.drawing_utils

# 입술의 랜드마크 인덱스를 정의
UPPER_LIP_INDICES = [61, 185, 40, 39, 37, 0, 267, 269, 270, 409, 415]
LOWER_LIP_INDICES = [146, 91, 181, 84, 17, 314, 405, 321, 375, 291, 308]

# 서버 설정
TCP_IP = '0.0.0.0' # 모든 인터페이스에서 수신
TCP_PORT = 65432 # 서버 포트 번호
BUFFER_SIZE = 1024

# 클라이언트 설정
#CLIENT_TCP_IP = '192.168.1.59'  # 서버 IP 주소
CLIENT_TCP_IP = '192.168.1.51'
CLIENT_TCP_PORT = 23458  # 서버 포트 번호

# 클라이언트 소켓을 저장할 리스트
clients = []
clients_lock = threading.Lock()

def is_mouth_open(landmarks, image_shape):
    """입이 열려있는지 확인하는 함수"""
    top_lip = landmarks[13]  # 윗입술 중앙
    bottom_lip = landmarks[14]  # 아랫입술 중앙

    top_lip_y = int(top_lip.y * image_shape[0])
    bottom_lip_y = int(bottom_lip.y * image_shape[0])

    # 입술 간의 거리 계산
    lip_distance = bottom_lip_y - top_lip_y

    # 특정 임계값 이상으로 입이 열려있으면 먹는 행동으로 간주
    return lip_distance > 20

def are_hands_above_shoulders(pose_landmarks, hand_landmarks, image_shape):
    """손이 어깨 위에 있는지 확인하는 함수"""
    if pose_landmarks and hand_landmarks:
        left_shoulder = pose_landmarks.landmark[mp_pose.PoseLandmark.LEFT_SHOULDER]
        right_shoulder = pose_landmarks.landmark[mp_pose.PoseLandmark.RIGHT_SHOULDER]

        shoulder_y = min(left_shoulder.y, right_shoulder.y) * image_shape[0]  # 어깨의 y 좌표

        for hand_landmark in hand_landmarks:
            for point in hand_landmark.landmark:
                if point.y * image_shape[0] < shoulder_y:
                    return True
    return False

def is_hand_closed(hand_landmarks):
    """손이 쥐어졌는지 확인하는 함수"""
    if hand_landmarks:
        for hand_landmark in hand_landmarks:
            thumb_tip = hand_landmark.landmark[mp_hands.HandLandmark.THUMB_TIP]
            index_finger_tip = hand_landmark.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]

            distance = abs(thumb_tip.x - index_finger_tip.x) + abs(thumb_tip.y - index_finger_tip.y)

            # 손가락 끝과 엄지 사이의 거리가 특정 임계값 이하인 경우 손이 쥐어졌다고 간주
            if distance < 0.05:
                return True
    return False

def handle_client(conn, addr):
    """클라이언트의 요청을 처리하는 함수"""
    global clients
    with clients_lock:
        clients.append(conn)  # 클라이언트를 리스트에 추가
    print(f"클라이언트와 연결되었습니다: {addr}")
    
    try:
        while True:  # 클라이언트와 데이터 통신
            data = conn.recv(BUFFER_SIZE).decode()
            if not data:
                break
            print(f"받은 데이터: {data}")

            # 모든 클라이언트에 데이터 전송
            with clients_lock:
                for client in clients:
                    if client != conn:  # 데이터를 보낸 클라이언트를 제외
                        client.send(data.encode())
    finally:
        with clients_lock:
            clients.remove(conn)  # 클라이언트 리스트에서 제거
        conn.close()  # 클라이언트와의 연결 종료
        print(f"클라이언트와의 연결이 종료되었습니다: {addr}")

def start_server():
    """서버를 시작하여 클라이언트로부터 데이터를 수신"""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((TCP_IP, TCP_PORT))
    server_socket.listen(5)  # 최대 5개의 클라이언트를 대기
    print("서버가 시작되었습니다. 클라이언트 연결을 기다리는 중...")
    
    while True:
        conn, addr = server_socket.accept()  # 클라이언트 연결 수립
        # 각 클라이언트에 대해 새로운 스레드 생성
        threading.Thread(target=handle_client, args=(conn, addr)).start()

    server_socket.close()  # 서버 소켓 종료

def client_process():
    """클라이언트 프로세스"""
    previous_mouth_open = False

    # 클라이언트 소켓 생성
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((CLIENT_TCP_IP, CLIENT_TCP_PORT))

    # 웹캠 설정
    cap = cv2.VideoCapture(0)

    with mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5) as pose, \
         mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5) as hands, \
         mp_face_mesh.FaceMesh(min_detection_confidence=0.5, min_tracking_confidence=0.5) as face_mesh:

        while cap.isOpened():
            res, image = cap.read()
            if not res:
                print('웹캠에서 이미지를 가져오는 것을 실패')
                break
            
            # BGR 이미지를 RGB로 변환
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

            # 속도 개선을 위한 이미지 쓰기 불가능
            image.flags.writeable = False

            # 포즈, 손 및 얼굴 인식
            pose_results = pose.process(image)
            hands_results = hands.process(image)
            face_results = face_mesh.process(image)

            # 이미지 다시 쓰기로 지정
            image.flags.writeable = True
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

            # 이미지의 크기(shape)를 저장
            image_shape = image.shape

            # 입이 열려 있는지 확인 (기본값은 False로 초기화)
            mouth_open = False

            motion_detected = False  # 동작 감지를 추적하는 플래그

            if face_results.multi_face_landmarks and pose_results.pose_landmarks and hands_results.multi_hand_landmarks:
                for face_landmarks in face_results.multi_face_landmarks:
                    # 현재 입이 열려 있는지 확인
                    mouth_open = is_mouth_open(face_landmarks.landmark, image_shape)
                    
                    # 입이 열려 있다가 닫히면 동작 감지로 간주
                    if previous_mouth_open and not mouth_open:
                        # 손이 어깨 위에 있고 쥐어져 있는지 확인
                        if are_hands_above_shoulders(pose_results.pose_landmarks, hands_results.multi_hand_landmarks, image_shape):
                            if is_hand_closed(hands_results.multi_hand_landmarks):
                                cv2.putText(image, "Clear", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 3, cv2.LINE_AA)
                                # TCP로 1 전송
                                message = "1"
                                client_socket.send(message.encode())
                                print("Motion detected, sent:", message)
                                # 프로그램 종료 준비
                                motion_detected = True
                                break
            
            if motion_detected:
                break  # 동작이 감지되면 루프 종료
            
            if not motion_detected:
                # 다른 조건이 충족되지 않았을 때 TCP로 0 전송
                message = "0"
                client_socket.send(message.encode())
                print("No motion detected, sent:", message)

            # 현재 입 상태를 이전 상태로 업데이트
            previous_mouth_open = mouth_open

            # 포즈와 손 랜드마크 그리기
            if pose_results.pose_landmarks:
                mp_drawing.draw_landmarks(
                    image, pose_results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

            if hands_results.multi_hand_landmarks:
                for hand_landmarks in hands_results.multi_hand_landmarks:
                    mp_drawing.draw_landmarks(
                        image, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # 이미지 좌우 반전 (글씨 포함)
            image = cv2.flip(image, 1)

            # 결과 화면에 출력
            cv2.imshow("frame", image)
            if cv2.waitKey(1) == ord('q'):
                break

    cap.release()
    client_socket.close()
    cv2.destroyAllWindows()

# 서버를 스레드로 실행
server_thread = threading.Thread(target=start_server)
server_thread.start()

# 클라이언트 프로세스 실행
client_process()

# 서버 스레드가 종료될 때까지 대기
server_thread.join()
