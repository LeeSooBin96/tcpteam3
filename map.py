import socket


HOST = '10.10.20.98'  # 서버의 IP 주소
PORT = 9106 # 서버의 포트 번호

# 서버에 연결
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))


# 서버로 메시지 전송
# message= "2013,광주"
# client_socket.sendall(message.encode())

# 서버로부터 데이터 수신
# data = client_socket.recv(1024)
# received_message = data.decode()
# print("Received message from server:", received_message)

# to_server = int(12345)
# right_method = to_server.to_bytes(4, byteorder='little')
# sent = csock.send(right_method)

# 연결 종료
client_socket.close()