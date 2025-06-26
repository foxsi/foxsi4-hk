import socket, sys, time
import select

local_ip = "192.168.1.8"
local_port = 7777
local_endpoint = (local_ip, local_port)
remote_ip = "192.168.1.16"
remote_port = 7777
remote_endpoint = (remote_ip, remote_port)


read_temp1_all =            bytes([0x01, 0xf2, 0x00])
read_temp2_all =            bytes([0x02, 0xf2, 0x00])
read_power_health_all =     bytes([0x04, 0x20, 0x00])
set_power_cdtede_off =      bytes([0x03, 0x00, 0x01])
set_power_cdtede_on =       bytes([0x03, 0x00, 0x00])
set_power_cdte1_off =       bytes([0x03, 0x03, 0x01])
set_power_cdte1_on =        bytes([0x03, 0x03, 0x00])
set_power_cdte2_off =       bytes([0x03, 0x04, 0x01])
set_power_cdte2_on =        bytes([0x03, 0x04, 0x00])
set_power_cdte3_off =       bytes([0x03, 0x05, 0x01])
set_power_cdte3_on =        bytes([0x03, 0x05, 0x00])
set_power_cdte4_off =       bytes([0x03, 0x06, 0x01])
set_power_cdte4_on =        bytes([0x03, 0x06, 0x00])
set_power_cmos1_off =       bytes([0x03, 0x08, 0x01])
set_power_cmos1_on =        bytes([0x03, 0x08, 0x00])
set_power_cmos2_off =       bytes([0x03, 0x07, 0x01])
set_power_cmos2_on =        bytes([0x03, 0x07, 0x00])
set_power_saas_off =        bytes([0x03, 0x02, 0x01])
set_power_saas_on =         bytes([0x03, 0x02, 0x00])
set_power_timepix_off =     bytes([0x03, 0x01, 0x01])
set_power_timepix_on =      bytes([0x03, 0x01, 0x00])
set_power_formatter_off =   bytes([0x03, 0x09, 0x01])
set_power_formatter_on =    bytes([0x03, 0x09, 0x00])
set_temp1_init =            bytes([0x01, 0xff, 0x00])
set_temp2_init =            bytes([0x02, 0xff, 0x00])
start_temp1_convert =       bytes([0x01, 0xf0, 0x00])
start_temp2_convert =       bytes([0x02, 0xf0, 0x00])
set_power_health_setup =    bytes([0x04, 0xff, 0x00])
set_intro_state_await =     bytes([0x07, 0xf0, 0x00])
set_intro_state_unlaunch =  bytes([0x07, 0xf0, 0x01])
set_intro_state_prelaunch = bytes([0x07, 0xf0, 0x02])
set_intro_state_shutter =   bytes([0x07, 0xf0, 0x03])
set_intro_state_end =       bytes([0x07, 0xf0, 0x04])
set_intro_reset_errors =    bytes([0x07, 0xe0, 0x00])

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.setblocking(0)
socket.bind(local_endpoint)
socket.connect(remote_endpoint)


if __name__ == "__main__":
    print("\nconfiguring...")
    socket.send(set_power_health_setup)
    time.sleep(0.1)
    socket.send(set_temp1_init)
    time.sleep(0.1)
    socket.send(set_temp2_init)
    time.sleep(0.1)
    socket.send(start_temp1_convert)
    time.sleep(0.1)
    socket.send(start_temp2_convert)
    time.sleep(1)

    print("running loop...")
    loop_counter = 0
    intercommand = 0.01
    timeout = 0.1
    while True:
        print("\tloop counter: ", loop_counter)
        socket.send(read_temp1_all)
        ready = select.select([socket], [], [], timeout)
        if ready[0]:
            data = socket.recv(1024)
            print(data.hex())

        time.sleep(intercommand)
        socket.send(read_temp2_all)
        ready = select.select([socket], [], [], timeout)
        if ready[0]:
            data = socket.recv(1024)
            print(data.hex())
        time.sleep(intercommand)
        socket.send(read_power_health_all, remote_endpoint)
        ready = select.select([socket], [], [], timeout)
        if ready[0]:
            data = socket.recv(1024)
            print(data.hex())
        time.sleep(intercommand)
        socket.send(start_temp1_convert)
        time.sleep(intercommand)
        socket.send(start_temp2_convert)
        
        print('\a')
        time.sleep(1.5)
        loop_counter = loop_counter + 1