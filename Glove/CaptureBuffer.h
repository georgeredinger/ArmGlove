
#ifndef CAPTUREBUFFER_H
#define CAPTUREBUFFER_H
#include <inttypes.h>

template <typename T, uint16_t Size>
class capture_buffer {
public:

  capture_buffer() : 
  start(0),end(0),size(Size+1) {  
  }  
  ~capture_buffer() {  
  }

  boolean empty(){
    return end == start;
  }

  T read(){
    T result = buf_[start];
    start = (start + 1) % (size);
    return result;
  }

  void write(T value) {
    buf_[end] = value;
    end = (end + 1) % (size);
    if (end == start)
      start = (start + 1) % (size); 
  }

private:
  int start;
  int end;
  int size;
  T buf_[Size+1];
};

#endif



