#ifndef BUFFER_H
#define BUFFER_H
#include <string>

class Buffer {
private:
  std::string buf;

public:
  Buffer();
  ~Buffer();

  void append(const char *_str, int _size);
  ssize_t size();
  const char *c_str();
  void clear();
  void getline();
  void setBuf(const char *);
};
#endif // BUFFER_H
