import socket

remote_address = '192.168.1.16'
remote_port = 7
print("create remote npoint")
remote_npoint = (remote_address,remote_port)
local_address = '192.168.1.20'
local_port = 7
print("create local npoint")
local_npoint = (local_address,local_port)
sock = socket.socket(socket.AF_INET)
print("bind to local")
sock.bind(local_npoint)
print("connect to remote")
sock.connect(remote_npoint)
print("start while loop")
while 1:
    data = input("input Data:")
    sock.send(data.encode())
    near_data=sock.recv(1024)
    print(near_data.hex())

