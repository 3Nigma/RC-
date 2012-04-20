#include "remrep.hpp"

RemRep::RemRep(const std::string &lports) : Replyer("self"), mListenPorts(lports) {
  mRMpool.push_back(this);

  // add index filter
  addGetFilter(std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)>>
               {"", [&](RequestInfo &ri, const std::string &procUri) {
    cJSON *root = echoIndex();
    mg_printf(ri.getClientStructure(), "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", cJSON_Print(root));
    cJSON_Delete(root);
  }});

  // add register filter
  addPostFilter(std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)>>
                {"register", [&](RequestInfo &ri, const std::string &procUri) {
    std::string localUri = procUri;
    if(localUri[0] == '/') localUri = localUri.substr(1);
    size_t delimPos = localUri.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    std::string objectName = (delimPos == std::string::npos ? localUri : localUri.substr(0, delimPos));

    RemoteObjectServer *newros = new RemoteObjectServer(objectName, ri.getClientFormattedIp(), "-1");
    //cJSON *root = echoIndex();
    mg_printf(ri.getClientStructure(), "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", ri.getCompleteUri());//cJSON_Print(root));
    //cJSON_Delete(root);
    mRMpool.push_back(newros);
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
  
/*bool RemRep::handlePostRequest(const RequestInfo &ri) {
  std::string processedUri = ri.getCompleteUri();

    /*if(processedUri == "/") processedUri = "/self";
    processedUri = processedUri.substr(1);  // go pass the '/' character

    if(processedUri.find(this->mName) == 0) {
      processedUri = processedUri.substr(this->mName.length());
      processedUri = processedUri.substr(1);  // go pass the '/' character

      // check method name
      if(processedUri.find("register") == 0) {
        processedUri = processedUri.substr(std::string("register").length());
        processedUri = processedUri.substr(1);  // go pass the '/' character

        if(processedUri.length() != 0) {
          // we have a new class to register
          char remoteInterface[10000];
          RemoteObjectServer *newROS = new RemoteObjectServer(processedUri, 
                                                              std::to_string(post_info->remote_ip),
                                                              std::to_string(post_info->remote_port));
          //mg_read(post_info, remoteInterface, sizeof(remoteInterface));
          newROS->setInterface(remoteInterface);
          mRMpool.push_back(newROS);
        }

      } else if (processedUri.find("unregister") == 0) {
        processedUri = processedUri.substr(std::string("unregister").length());
        processedUri = processedUri.substr(1);  // go pass the '/' character

      } else
        return false;
    }

  return true;
}*/

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
  const std::string requestType = request_info->request_method;
  RemRep *repo = (RemRep *)request_info->user_data;

  switch(event) {
  case MG_NEW_REQUEST:  // New HTTP request has arrived from the client
    std::cout << "Client [" << ri.getClientFormattedIp() << ":" << ri.getClientPort() << "] request inbound : " 
              << requestType << " " << ri.getCompleteUri() << std::endl;
    if(requestType == "GET") {
      if(repo->handleGetRequest(ri) == false) {
        // there was an error in the request
        std::cout << " * Invalid request ... routing to index." << std::endl;
        mg_printf(ri.getClientStructure(), "HTTP/1.1 301 Moved Permanently\r\n"
                                           "Location: /\r\n");
      }
    } else if(requestType == "POST")
      repo->handlePostRequest(ri);
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
