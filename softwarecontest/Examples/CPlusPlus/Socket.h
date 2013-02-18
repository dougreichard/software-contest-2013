#pragma once

#include "../../include/libuv/uv.h"
#include <string>
using namespace std;

class Socket 
{
public:
	uv_tcp_t * getSocket();

	void BeginRead();
	void BeginWrite(const string& s);
	void Close();


	void BeginWrite(uv_buf_t buf);
	virtual void OnString(string s);
	virtual void OnData(ssize_t nread, uv_buf_t buf);
	virtual void OnClose() ;
	virtual void OnError(string s);
	virtual void OnReadError();
	virtual void OnDisconnect();
	virtual void OnWriteError();
	virtual void OnConnectionError();
		
protected:
	static void socket_read(uv_stream_t *socket, ssize_t nread, uv_buf_t buf);
	static uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size);
	static void after_write(uv_write_t *req, int status);
	static void on_close(uv_handle_t *server); 
private:
	uv_tcp_t _socket;
};

class ServerSocket;
class ClientSocket : public Socket
{
public:
	ClientSocket();

	void BeginConnect(uv_loop_t* loop, string addr, int port);
	virtual void OnConnect();
protected:
	static void on_connect(uv_connect_t* req, int status);
};



class ServerSocket : public Socket
{
public:
	void Listen(uv_loop_t* loop, int port=8124);
protected:
	static void on_new_connection(uv_stream_t *server, int status);
	
	// Events
	virtual void OnListening();
	virtual void OnListeningError() {}
	virtual void OnConnection(ClientSocket* );
	virtual ClientSocket* AllocClient();

	ClientSocket* Accept();

};