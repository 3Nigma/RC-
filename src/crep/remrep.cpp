#include "remrep.hpp"

RemRep::RemRep(const std::string &lports) : Replyer("self"), mListenPorts(lports) {
  mRMpool.push_back(this);

  // add register filter
  addPostFilter(std::tuple<std::string, std::function<cJSON *(RequestInfo &)>>
                {"register", [&](RequestInfo &ri) -> cJSON* {
    cJSON *root = nullptr;
    char requestBody[32768];
    RemoteObjectServer *newros = new RemoteObjectServer();

    mg_read(ri.getClientStructure(), requestBody, sizeof(requestBody));
    try {
      newros->parseJString(requestBody);
      mRMpool.push_back(newros);
      addStatusMsg(&root, "Registration succeded : Object indexed into service.");
    } catch(BadJSONFormatException) {
      addStatusMsg(&root, "Registration canceled : Unable to parse received data!");  
    }

    return root;
  }});
}

RemRep::~RemRep() {
  StopRepServer();
  mRMpool.clear();
}

void RemRep::StartRepServer() {
  const char *options[] = {"listening_ports", mListenPorts.c_str(), NULL};

  mCtx = mg_start(&WebCallback, this, options);
}

void RemRep::StopRepServer() {
  mg_stop(mCtx);
}

cJSON *RemRep::handleObjectRequest(RequestInfo &ri, const std::string &requestType) {
  cJSON *root = nullptr;
  bool objFound = false;
  std::string objReqName = ri.getRequestObjectName();

  for(Replyer *rCandidate : mRMpool)
    if(rCandidate->getName() == objReqName) {
      if(requestType == "GET") {
        root = rCandidate->handleGetRequest(ri);
      } else if(requestType == "POST")
        root = rCandidate->handlePostRequest(ri);
      objFound = true;
    }

  if(!objFound)
    addStatusMsg(&root, "Unable to process request : Unrecognized OBJECT.");  

  return root;
}

cJSON *RemRep::echoIndexHook() {
  cJSON *root = cJSON_CreateObject();
  cJSON *objs = nullptr;

  cJSON_AddNumberToObject(root, "objectcount", mRMpool.size());
  cJSON_AddItemToObject(root, "objects", objs = cJSON_CreateArray()); 
  std::for_each(mRMpool.begin(), mRMpool.end(), [&](Replyer *ros) {
    cJSON_AddItemToArray(objs, cJSON_CreateString(ros->getName().c_str()));
  });

  return root;
}

void *RemRep::WebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) {
/* This structure contains information about the HTTP request.
struct mg_request_info {
  void *user_data;       // User-defined pointer passed to mg_start()
  char *request_method;  // "GET", "POST", etc
  char *uri;             // URL-decoded URI
  char *http_version;    // E.g. "1.0", "1.1"
  char *query_string;    // URL part after '?' (not including '?') or NULL
  char *remote_user;     // Authenticated user, or NULL if no auth used
  char *log_message;     // Mongoose error log message, MG_EVENT_LOG only
  long remote_ip;        // Client's IP address
  int remote_port;       // Client's port
  int status_code;       // HTTP reply status code, e.g. 200
  int is_ssl;            // 1 if SSL-ed, 0 if not
  int num_headers;       // Number of headers
  struct mg_header {
    char *name;          // HTTP header name
    char *value;         // HTTP header value
  } http_headers[64];    // Maximum 64 headers
};
*/
  cJSON *root = nullptr;
  RequestInfo ri(client, request_info);
  const std::string reqType = request_info->request_method;
  const std::string reqUri = ri.getCompleteUri();
  RemRep *repo = (RemRep *)request_info->user_data;

  switch(event) {
  case MG_NEW_REQUEST:  // New HTTP request has arrived from the client
    if(reqUri.length() == 1) {
      mg_printf(ri.getClientStructure(), "HTTP/1.1 301 Moved Permanently\r\n"
                "Location: /self\r\n");
    } else {
      std::cout << "Client [" << ri.getClientFormattedIp() << ":" << ri.getClientPort() << "] request inbound : " 
                << reqType << " '" << reqUri << "'" << std::endl;
      root = repo->handleObjectRequest(ri, reqType);
      mg_printf(ri.getClientStructure(), "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "%s", cJSON_Print(root));
    }
    return (void *)"";  // Mark as processed
    break;
  case MG_HTTP_ERROR:  // HTTP error must be returned to the client
    break;
  case MG_EVENT_LOG:  // Mongoose logs an event, request_info.log_message
    break;
  case MG_INIT_SSL:  // Mongoose initializes SSL. Instead of mg_connection *,
                     // SSL context is passed to the callback function.
    break;
  case MG_REQUEST_COMPLETE:  // Mongoose has finished handling the request
    break;
  }

  return NULL;
}
