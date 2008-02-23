import socket
 
SERVER_ADDRESS='127.0.0.1'
SERVER_PORT=10000
 
client=socket.socket(socket.AF_INET,socket.SOCK_DGRAM,socket.IPPROTO_UDP)
 
for i in range(3):
        print 'Sending packet %d' % i
        message = 'This is packet %d' % i
        client.sendto(message,(SERVER_ADDRESS,SERVER_PORT))
 
client.close()

