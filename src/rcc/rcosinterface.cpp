#include "rcosinterface.hpp"

RCOSInterface::RCOSInterface(const std::string &codeFile) {
  std::ifstream fCode(codeFile.c_str());

  if(fCode.good()) {
    mHRawContent.assign(std::istreambuf_iterator<char>(fCode), std::istreambuf_iterator<char>());
    std::cout << "Raw header content : " << std::endl
              << mHRawContent << std::endl;
  } else
    std::cerr << "Unable to open header file '" << codeFile << "'!" << std::endl;

  fCode.close();
}

void RCOSInterface::saveRCOS(std::string dir) {
  if(dir[dir.length() - 1] != '/') dir += "/";
  std::ofstream rcosResult(dir + getFileSaveName());

  rcosResult << mSContent;
  rcosResult.close();
}

std::string RCOSInterface::getFileSaveName() {
  std::string fileNameChunk = mServerClassName;
  //std::transform(fileNameChunk.begin(), fileNameChunk.end(), fileNameChunk.begin(), ::tolower);

  return std::string("rcos_") + fileNameChunk + std::string(".cpp");
}
