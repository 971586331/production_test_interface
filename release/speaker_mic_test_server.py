import BaseHTTPServer
import urlparse
import time
from SocketServer import ThreadingMixIn
import threading
import os

import SimpleHTTPServer
import SocketServer

RECORD_FILE = "record-16KHz-16bit-Stereo.pcm"
RECORD_START = "MIC:START"

class WebRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def do_POST(self):
        #print 'post message'
        parsed_path = urlparse.urlparse(self.path)
        length = self.headers.getheader('content-length');
        nbytes = int(length)
        data = self.rfile.read(nbytes)
        cur_thread = threading.currentThread()
        #print 'Thread:%s\tdata:%s' % (cur_thread.getName(), data)

        if nbytes == len(RECORD_START) and data.decode('utf-8') == RECORD_START:
            print 'Factory test: mic start, remove old file'
            if os.path.exists(RECORD_FILE):
                os.remove(RECORD_FILE)
        else:
            with open(RECORD_FILE.decode('utf-8'),'ab+') as f:
                f.write(data)

        message_parts = [ 'just a test']
        message = '\r\n'.join(message_parts)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(message)

class ThreadingHttpServer( ThreadingMixIn, BaseHTTPServer.HTTPServer ):
    pass


class StartSimpleHTTPServer:

  def __init__(self, port):
    self.port = port

  def start_server(self):
	Handler = SimpleHTTPServer.SimpleHTTPRequestHandler
	httpd = SocketServer.TCPServer(("", self.port), Handler)
	httpd.serve_forever()

if __name__ == '__main__':
    #server = BaseHTTPServer.HTTPServer(('0.0.0.0',18460), WebRequestHandler)  
    server = ThreadingHttpServer(('0.0.0.0',22808), WebRequestHandler)
    ip, port = server.server_address
    # Start a thread with the server -- that thread will then start one
    # more thread for each request
    mic_server_thread = threading.Thread(target=server.serve_forever)
    # Exit the server thread when the main thread terminates
    mic_server_thread.setDaemon(True)
    mic_server_thread.start()
    print "Server loop running in thread:", mic_server_thread.getName()

    speaker_server = StartSimpleHTTPServer(12800)
    speaker_server_thread = threading.Thread(target=speaker_server.start_server)
    # Exit the server thread when the main thread terminates
    speaker_server_thread.setDaemon(True)
    speaker_server_thread.start()
    print "Server loop running in thread:", speaker_server_thread.getName()

    while True:
        time.sleep(10)
        pass

