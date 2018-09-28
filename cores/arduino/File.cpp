/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2015 SkyTraq

 */

#include "SD.h"

extern char dbg_buf[1024];

File::File(SdFile& f, const char *n) {
  // oh man you are kidding me, new() doesnt exist? Ok we do it by hand!
  _file = new SdFile(f);
  if (_file) {
    strncpy(_name, n, MAX_PATH_LEN - 1);
    _name[MAX_PATH_LEN - 1] = 0;
  }  
}

File::File(void) {
  _file = 0;
  _name[0] = 0;
}

File::~File() {
  if (_file) {
    delete _file; 
    _file = 0;
  }
}

// returns a pointer to the file name
char *File::name(void) {
  return _name;
}

// a directory is a special type of file
boolean File::isDirectory(void) {
  return (_file && _file->isDir());
}

size_t File::write(uint8_t val) {
  return write(&val, 1);
}

size_t File::write(uint8_t *buf, size_t size) {
  if (!_file) {
    return 0;
  }

  _file->clearWriteError();
  size_t t = _file->write(buf, size);
  if (_file->getWriteError()) {
    return 0;
  }
  return t;
}

int File::peek() {
  if (!_file) 
    return -1;
  
  int c = _file->read();
  if (c != -1) _file->seekCur(-1);
  return c;
}

int File::read() {
  if (_file) 
    return _file->read();
  return -1; 
}


// buffered read for more efficient, high speed reading
int File::read(void *buf, uint16_t nbyte) {
return -1;
/*  
  if (_file) 
    return _file->read(buf, nbyte);
  return 0;
*/
}

size_t File::available() {
  if (!_file) return 0;
  uint32_t n = size() - position();
  return n > 0X7FFF ? 0X7FFF : n;
}

void File::flush() {
  if (_file)
    _file->sync(); 
}

boolean File::seek(uint32_t pos) {
  if (! _file) return false;
  return _file->seekSet(pos);
}

uint32_t File::position() {
  if (! _file) return 0;
  return _file->curPosition();
}

uint32_t File::size() {
  if (!_file) return 0;
  return _file->curSize();
}

void File::close() {
  if (_file) {
    _file->close();
    delete _file; 
    _file = 0;
  }
}

File::operator bool() {
  if (_file) 
    return  _file->isOpen();
  return false;
}


