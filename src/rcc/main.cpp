#include <iostream>
#include <cstdio>
#include <cJSON/cJSON.h>
#include "mongoose/mongoose.h"
#include "curl/curl.h"

#define RCC

class ObjectServer {
public:
  ObjectServer() = default;

  void RegisterObject(const std::string &serverAddress, unsigned int localPort) {
    CURL *repoLink = nullptr;
    cJSON *root = cJSON_CreateObject();

    std::string serverUrl = "http://" + serverAddress + "/self/register?ObjectName";

    if((repoLink = curl_easy_init()) != nullptr) {
      // create register body
      cJSON_AddStringToObject(root, "serverip", "127.0.0.1");
      cJSON_AddNumberToObject(root, "serverport", localPort);
      cJSON_AddStringToObject(root, "interface", "#include <iostream> ...");

      curl_easy_setopt(repoLink, CURLOPT_URL, serverUrl.c_str());
      curl_easy_setopt(repoLink, CURLOPT_POSTFIELDS, cJSON_Print(root));
      curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply);
      curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, NULL);

      curl_easy_perform(repoLink);

      curl_easy_cleanup(repoLink);
    }
  }
protected:
  static void *WebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) {

  }

  static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *destStream) {
    printf("%s\n", (char *)ptr);
    return elCount;
  }
};

int main(int argc, char *argv[]) {
  ObjectServer os;

  os.RegisterObject("127.0.0.1:8081", 93);

  return 0;
}
