#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>
#include <iterator>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include "b64.h"
#include "WebServer.h"

unsigned nalIdx = 0;

// Handler for when a message is received from the client
void WebServer::onMessageReceived(int clientSocket, const char* msg, int length)
{
	// Parse out the client's request string e.g. GET /index.html HTTP/1.1
	std::istringstream iss(msg);
	std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
	// std::cout << msg << std::endl;
	// Some defaults for output to the client (404 file not found 'page')
	std::string content = "<h1>404 Not Found</h1>";
	std::string requestedFile = "/index.html";
	contentType = "text/html";
	int errorCode = 404;
	int stream = 0;
	// If the GET request is valid, try and get the name
	if (parsed.size() >= 3 && parsed[0] == "GET") {

		requestedFile = parsed[1];
		std::cout << parsed[6] << " asks for " << parsed[1] << std::endl; 
		// If the file is just a slash, use index.html. This should really
		// be if it _ends_ in a slash. I'll leave that for you :)
		if (requestedFile == "/")
			requestedFile = "/index.html";

		stream = MIMEType(clientSocket, &requestedFile);

	}
	
	std::ostringstream oss;

	if (!stream) {
		// Open the document in the local file system
		std::ifstream f("../wwwroot" + requestedFile);

		// Check if it opened and if it did, grab the entire contents
		if (f.good()) {
			std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			content = str;
			errorCode = 200;
		}

		f.close();

		// Write the document back to the client
		oss << "HTTP/1.1 " << errorCode << " OK\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Content-Type: "<< contentType << "\r\n";
		oss << "Content-Length: " << content.size() << "\r\n";
		oss << "\r\n";
		oss << content;


	} else {
		nalIdx = -1;

		// int f = open("/home/ckevar/Downloads/video.264.0000", O_RDONLY);
		// unsigned char data[28];
		// unsigned char *tmp = data;

		// if (f > 0) {
		// 	while(1) {			
		// 		ssize_t len2 = read(f, tmp, 28);
		// 		if (len2 <= 0) break;
		// 		tmp += len2;
		// 	}

		// 	errorCode = 200;
		// }

		// close(f);

		// char *enc = b64_encode(data, tmp - data);
		unsigned char data[2] = {65, 66};
		char *enc = b64_encode(data, 2);

		errorCode = 200;
		oss << "HTTP/1.1 " << errorCode << " OK\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Content-Type: "<< contentType << "\r\n";
		oss << "data: "<< enc; 
		oss << "\n\n";
		free(enc);
		nalIdx++;
	}

	std::string output = oss.str();
	int size = output.size() + 1;
	sendToClient(clientSocket, output.c_str(), size);
}


// Handler for when time out on poll,
void WebServer::onTimeOut() {
	char file[50];
	sprintf(file, "%s.%04d", "/home/ckevar/Downloads/yyyyyyy.264", nalIdx);
	int f = open(file, O_RDONLY);
	unsigned char data[102346];
	unsigned char *tmp = data;

	if (f > 0) {
		while(1) {
			ssize_t len2 = read(f, tmp, 11);
			if (len2 <= 0) break;
			tmp += len2;
		}
	}

	close(f);

	char *enc = b64_encode(data, tmp - data);

	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/event-stream\r\n";
	oss << "data: " << enc;	
	oss << "\n\n";

	std::string output = oss.str();
	int size = output.size() + 1;
	streamToClients(output.c_str(), size);

	if (nalIdx > 1435) 
		nalIdx = 0;
		// deallocateStreamingAllClients();

	free(enc);
	nalIdx++;
}

// Handler for client connections
void WebServer::onClientConnected(int clientSocket) {

}

// Handler for client disconnections
void WebServer::onClientDisconnected(int clientSocket) {
	
}

int WebServer::MIMEType(int cSocket, std::string *rType) {
	int idx = rType->size();
	while (rType->at(idx - 1) != '.') idx--; 
	std::string mimetype = rType->substr(idx);

	if (mimetype == "html")
		contentType = "text/html";
	
	else if (mimetype == "js") 
		contentType = "text/javascript";

	else if (mimetype == "json") 
		contentType = "text/javascript";

	else if (mimetype == "map") 
		contentType = "text/javascript";

	else if (mimetype == "css") 
		contentType = "text/css";

	else if (mimetype == "jpeg" || mimetype == "jpg") 
		contentType = "image/jpeg";
	
	else if (mimetype == "png") 
		contentType = "image/png";

	else if (mimetype == "mp4")
		contentType = "video/mp4";

	else if (mimetype == "ts") {
		contentType = "text/event-stream";
		allocateStreaming(cSocket);
		return 1;
	}

	return 0;
}

