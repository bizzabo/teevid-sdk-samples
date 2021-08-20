#ifndef TEEVID_SDK_FILEINFO_H
#define TEEVID_SDK_FILEINFO_H

#include <string>

struct FileInfo
{
  unsigned int size;       // in bytes
  std::string title;       // file name provided during uploading
  std::string fileName;    // unique file name: UNIX timestamp + extention
  std::string mimeType;    // file type/extention
  std::string type;        // file extention
  unsigned long createdTs; // UNIX timestamp in milliseconds
  std::string owner;       // owner's name
  std::string filepath;    // relative path
  std::string downloadUrl; // link to share
};

#endif //TEEVID_SDK_FILEINFO_H
