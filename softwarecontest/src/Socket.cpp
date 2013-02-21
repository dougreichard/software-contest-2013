#include "uv.h"
#include "Socket.h"
#include <iostream>


uv_tcp_t * Socket::getSocket() {
	return &_socket;
}

void Socket::BeginRead() {
	uv_read_start((uv_stream_t*)&_socket, alloc_buffer, socket_read);
}

void Socket::BeginWrite(const string& s) {
	uv_buf_t buf = uv_buf_init((char*) malloc(s.length()), s.length());
	s.copy(buf.base,s.length());
	BeginWrite(buf);
}


void Socket::BeginWrite(uv_buf_t buf) {
	uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
	req->data = (void*) buf.base;
	uv_write(req,	(uv_stream_t *)	&_socket, &buf, 1, after_write);
}
	
void Socket::OnString(string s) {
	cout << "Length:" << s.length() << '\n';

	BeginWrite(s);

	cout << "I got the message" << '\n';
}

void Socket::OnData(ssize_t nread, uv_buf_t buf) {
	std::string s(buf.base, nread);
	OnString(s);
}
void Socket::OnClose() {
}
void Socket::OnError(string s) {}
void Socket::OnReadError() {OnError("Read");}
void Socket::OnWriteError() {OnError("Write");}
void Socket::OnConnectionError() {OnError("Connection");}
		
void Socket::socket_read(uv_stream_t *socket, ssize_t nread, uv_buf_t buf) {
	Socket* socketClass = (Socket*)(socket->data);
	if (nread == -1) {
		uv_err_t err = uv_last_error(socket->loop);

		if (err.code == UV_EOF) {
			return;
		} else if (err.code == UV_ECONNRESET) {
			socketClass->OnDisconnect();
			return;
		} else {
			fprintf(stderr, "Read error %s\n", uv_err_name(uv_last_error(socket->loop)));
			socketClass->OnReadError();
		}
		return;
	}
		
	socketClass->OnData(nread, buf);
	free(buf.base);
	
}

uv_buf_t Socket::alloc_buffer(uv_handle_t *handle, size_t suggested_size) {
	return uv_buf_init((char*) malloc(suggested_size), suggested_size);
}
// Called after a write to 'clean up' anything
void Socket::after_write(uv_write_t *req, int status) {
		// Not sure this is the right handle
		Socket* socketClass = (Socket*)(req->handle->data);
		if (status == -1) {

		socketClass->OnWriteError();

		cout << "Write error" << uv_err_name(uv_last_error(req->handle->loop));
	}
	
	char *base = (char*) req->data;
	free(base);
	free(req);
}

void Socket::on_close(uv_handle_t *server) {
	Socket* that = (Socket*)server->data;
	that->OnClose();
}

void Socket::Close() {
	uv_close((uv_handle_t*) getSocket(), on_close);
}

void Socket::OnDisconnect() {
}

ClientSocket* ServerSocket::Accept() {
	ClientSocket* client = AllocClient();
	uv_tcp_init(getSocket()->loop, client->getSocket());
	if(uv_accept((uv_stream_t*)getSocket(), (uv_stream_t*) client->getSocket()) == 0) {
		client->getSocket()->data = client;
		OnConnection(client);
	}
	  else {
		OnError("Accepting Client");
        client->Close();
    }
	return client;
}


void ServerSocket::Listen(uv_loop_t* loop, int port) {
	uv_tcp_init(loop, getSocket());

	// Loby port 8124
	struct sockaddr_in lobby_addr = uv_ip4_addr("0.0.0.0", port);
	uv_tcp_bind(getSocket(), lobby_addr);

	getSocket()->data = this;

	int result = uv_listen((uv_stream_t*)getSocket(), 128, on_new_connection);
	if (result) {
		OnListeningError ();

		cerr << "Lobby server could not listen\n" << uv_err_name(uv_last_error(loop));
	}
	else OnListening();
}

void ServerSocket::OnListening() {
	cout << "Created Lobby" << '\n';
}



void ServerSocket::on_new_connection(uv_stream_t *server, int status) {
	ServerSocket* serSock = (ServerSocket*)server->data;
	if (status == -1) {
		// Error close the lobby
		cout << "connection error" << '\n';
		serSock->OnConnectionError();
		return;
	}
	cout << "connection" << '\n';

	ClientSocket* client = serSock->Accept();
	if (client!=NULL) {
		client->BeginRead();
	}
	  else {
		  client->Close();
    }

}
void ServerSocket::OnConnection(ClientSocket* ) {
}

ClientSocket* ServerSocket::AllocClient() {
	return new ClientSocket();
}





ClientSocket::ClientSocket()
{
	//uv_tcp_init(loop, getSocket());
	getSocket()->data = this;
}


void ClientSocket::BeginConnect(uv_loop_t* loop, string addr, int port) {
	
	uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	uv_tcp_init(loop, getSocket());
	getSocket()->data = this;

	struct sockaddr_in dest = uv_ip4_addr(addr.c_str(), port);
	uv_tcp_connect(connect, getSocket(), dest, on_connect);
}

void ClientSocket::on_connect(uv_connect_t* req, int status) {
	ClientSocket* client = (ClientSocket*)req->handle->data;
	// Free Memory
	free(req);



	if (status == -1) {
		// Error close the lobby
		cout << "connect error" << '\n';
		client->OnConnectionError();
		return;
	}
	cout << "connect to server" << '\n';
	
	if (client!=NULL) {
		client->OnConnect();
	}
}

void ClientSocket::OnConnect() {
	
}
