#include <iostream>
#include <list>
#include <cstdio>
#include <ctime>
#include "mongoose/mongoose.h"
#include "cJSON/cJSON.h"

class Replyer {
public:
  Replyer(const std::string &name) : mName(name) {
    time_t rawtime;

    time (&rawtime);
    mCreationTime = localtime(&rawtime);
  }
 
  virtual bool handleGetRequest(struct mg_connection *client, const std::string &uri) = 0;
protected:
  std::string mName;
  struct tm *mCreationTime;

  virtual cJSON *echoIndexHook() {
    return nullptr;
  }

  virtual cJSON *echoIndex() {
    std::string formattedCreationTime = asctime(mCreationTime);
    cJSON *root = cJSON_CreateObject();
    cJSON *childSpecRoot = nullptr;

    cJSON_AddItemToObject(root, "objectname", cJSON_CreateString(mName.c_str()));
    cJSON_AddItemToObject(root, "createdon", cJSON_CreateString(formattedCreationTime.substr(0, formattedCreationTime.length()-1).c_str()));

    if((childSpecRoot = echoIndexHook()) != nullptr)
      cJSON_AddItemReferenceToObject(root, "hookreport", childSpecRoot);

    return root;
  }
};

class RemoteObjectServer : public Replyer {
public:
  RemoteObjectServer(const std::string &name, const std::string &ip, const std::string &port)
    : Replyer(name), mIp(ip), mPort(port), mInReturnCount(0) { }
  
  void setInterface(const std::string &in) { mInterface = in; }
  const std::string getInterface() { 
    mInReturnCount++;
    return mInterface;
  }
  unsigned int getInterfaceReturnCnt() { return mInReturnCount; }
  cJSON *echoInterface() {
    cJSON *root = nullptr;
    cJSON *in = nullptr;

    cJSON_AddItemToObject(in, "objectname", cJSON_CreateString(mName.c_str()));
    cJSON_AddItemToObject(root, "interface", in = cJSON_CreateObject());
    cJSON_AddItemToObject(in, "c++", cJSON_CreateString(mInterface.c_str()));

    return root;
  }

  virtual bool handleGetRequest(struct mg_connection *client, const std::string &uri) {
    return true;
  }

protected:
  std::string mIp;
  std::string mPort;
  std::string mInterface;
  unsigned int mInReturnCount;
};

class RemRep : public Replyer {
public:
  RemRep(const std::string &lports) : Replyer("self"), mListenPorts(lports) {
    mRMpool.push_back(this);
  }
  ~RemRep() {
    StopRepServer();
    mRMpool.clear();
  }

  void StartRepServer() {
    const char *options[] = {"listening_ports", mListenPorts.c_str(), NULL};

    mCtx = mg_start(&WebCallback, this, options);
  }
  void StopRepServer() {
    mg_stop(mCtx);
  }
  
  bool handlePostRequest(const struct mg_request_info *post_info) {
    std::string processedUri = post_info->uri;

    if(processedUri == "/") processedUri = "/self";
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
  }
  virtual bool handleGetRequest(struct mg_connection *client, const std::string &uri) {
    std::string processedUri = uri;

    if(uri == "/") processedUri = "/self";
    processedUri = processedUri.substr(1);  // go pass the '/' character

    if(processedUri.find(this->mName) == 0) {
      cJSON *root = echoIndex();
      mg_printf(client, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", cJSON_Print(root));
      cJSON_Delete(root);
    }
  
    return true;
  }

protected:
  std::list<Replyer *> mRMpool;
  std::string mListenPorts;
  struct mg_context *mCtx;

  virtual cJSON *echoIndexHook() {
    return nullptr;
  }

  static void *WebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) {
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
    const std::string requestType = request_info->request_method;
    RemRep *repo = (RemRep *)request_info->user_data;

    switch(event) {
    case MG_NEW_REQUEST:  // New HTTP request has arrived from the client
      std::cout << "Client [" << request_info->remote_ip << ":" << request_info->remote_port << "] request inbound : " 
                << requestType << " " << request_info->uri << std::endl;
      if(requestType == "GET")
        repo->handleGetRequest(client, request_info->uri);
      else if(requestType == "POST")
        repo->handlePostRequest(request_info);
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
};

int main(int argc, char *argv[]) {
  RemRep *rep = new RemRep("8081");

  rep->StartRepServer();
  getchar();
  delete rep;
  return 0;
}
