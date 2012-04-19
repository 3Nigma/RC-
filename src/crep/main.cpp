#include <iostream>
#include <cstdio>
#include "mongoose/mongoose.h"
#include "cJSON/cJSON.h"

class RemRep {
public:
  RemRep(const std::string &lports) : mListenPorts(lports) {}
  ~RemRep() {
    StopRepServer();
  }

  void StartRepServer() {
    const char *options[] = {"listening_ports", mListenPorts.c_str(), NULL};

    mCtx = mg_start(&WebCallback, NULL, options);
  }
  void StopRepServer() {
    mg_stop(mCtx);
  }
  
protected:
  std::string mListenPorts;
  struct mg_context *mCtx;

  static void *WebCallback(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info) {
    cJSON *root = nullptr;
    
    switch(event) {
    case MG_NEW_REQUEST:
      root = cJSON_CreateObject();
      cJSON_AddItemToObject(root, "name", cJSON_CreateString("Vic cel mic!"));
      mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", cJSON_Print(root));//request_info->uri);
      return (void *)"";  // Mark as processed
      break;
    default:
      break;
    }

    return NULL;
  }
};

int main(int argc, char *argv[]) {
  RemRep *rep = new RemRep("8080");

  rep->StartRepServer();
  getchar();
  delete rep;
  return 0;
}
