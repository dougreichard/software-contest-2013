// CPlusPlus.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../include/libuv/uv.h"
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/Writer.h"
#include <iostream>
#include <sstream>
#include "Socket.h"

using namespace std;
using namespace rapidjson;
// Windows only
#pragma comment(lib, "../../lib/libuv.lib")



class MyStringStream {
public:
	void Put(char c) { ss << c; }

	// Not implemented
	char* PutBegin() { return 0; }
	size_t PutEnd(char*) { return 0; }
	string str() {return ss.str();}

private:
	ostringstream ss;
};



class MyPlayer : public ClientSocket {
public:
	void Play(uv_loop_t* loop) {
		BeginConnect(loop, "127.0.0.1", 8124);
	}

	virtual void OnString(string s) {
		// For Writing JSOn
		MyStringStream ss;
		Writer<MyStringStream> writer(ss);

		// Parse Json
		Document doc;

		doc.Parse<0>(s.c_str());
		if (doc.HasParseError()) {
			// ERROR 
		} else if (doc.HasMember("id")) {
			// Game Start
			int otherguy = doc["id"].GetInt();
			int myPlayer = doc["player"].GetInt();

			writer.StartObject();
			writer.String("ready");
			writer.Bool(true);
			writer.EndObject();

			BeginWrite(ss.str());

		}  else if (doc.HasMember("card")) {
			// Got Card
			cout << "got a card" << s;
			cout << "I'm feeling down";

			writer.StartObject();
			writer.String("push");
			writer.String("D");
			writer.EndObject();

			BeginWrite(ss.str());

		}  else if (doc.HasMember("winner")) {
			// Got Game State
			cout << "Game State" << s;
			
			writer.StartObject();
			writer.String("ready");
			writer.Bool(true);
			writer.EndObject();

			BeginWrite(ss.str());

		} else {
			cout << "Game bad data??" << s;
		}
	}

	virtual void OnDisconnect() {
		uv_unref((uv_handle_t*)getSocket());
	}
	virtual void OnClose() {
		uv_unref((uv_handle_t*)getSocket());
	}
	virtual void OnConnect() {
		cout << "Said my name droopy";

		MyStringStream ss;
		Writer<MyStringStream> writer(ss);

		writer.StartObject();
		writer.String("name");
		writer.String("droopy");
		writer.EndObject();

		BeginWrite(ss.str());
		BeginRead();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	uv_loop_t* loop = uv_loop_new();

	MyPlayer player;
	player.Play(loop);
	
	
	uv_run(loop, UV_RUN_DEFAULT);
	cout << "Goodbye!" << '\n';
	
	return 0;
}

