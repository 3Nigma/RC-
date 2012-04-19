#include <iostream>
#include <cstdio>
#include "mongoose/mongoose.h"
#include "cJSON/cJSON.h"

void *callback(enum mg_event event,
	       struct mg_connection *conn,
	       const struct mg_request_info *request_info) {
  if (event == MG_NEW_REQUEST) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", cJSON_Print(root));//request_info->uri);
    return (void *)"";  // Mark as processed
  } else {
    return NULL;
  }
}

int main(int argc, char *argv[]) {
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", NULL};

  ctx = mg_start(&callback, NULL, options);
  getchar();
  mg_stop(ctx);

  return 0;
}
