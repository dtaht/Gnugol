import socket
 
PORT = 10000
BUFLEN = 512
 
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
server.bind(('', PORT))
 
while True:
        (message, address) = server.recvfrom(BUFLEN)
        print 'Received packet from %s:%d' % (address[0], address[1])
        print 'Data: %s' % message
