#pragma once

#include "WString.h" 
#include "Stream.h" 
#include "diskio.h" 
#include "ff.h" 

// flags for timestamp
/** set the file's last access date */
uint8_t const T_ACCESS = 1;
/** set the file's creation date and time */
uint8_t const T_CREATE = 2;
/** Set the file's write date and time */
uint8_t const T_WRITE = 4;
// values for type_
/** This SdFile has not been opened. */
uint8_t const FAT_FILE_TYPE_CLOSED = 0;
/** SdFile for a file */
uint8_t const FAT_FILE_TYPE_NORMAL = 1;
/** SdFile for a FAT16 root directory */
uint8_t const FAT_FILE_TYPE_ROOT16 = 2;
/** SdFile for a FAT32 root directory */
uint8_t const FAT_FILE_TYPE_ROOT32 = 3;
/** SdFile for a subdirectory */
uint8_t const FAT_FILE_TYPE_SUBDIR = 4;
/** Test value for directory type */
uint8_t const FAT_FILE_TYPE_MIN_DIR = FAT_FILE_TYPE_ROOT16;

#define SD_CHIP_SELECT_PIN    28
#define MAX_PATH_LEN      260

class SdFile : public Print {
protected:
  char _name[MAX_PATH_LEN]; // our name
  FIL _fl;
  DIR _di;
  FILINFO _fi;
  bool _isDir;
  FRESULT _result;
  
public:
  SdFile();
  SdFile(const SdFile& f);

  bool isDir();
  FRESULT getWriteError();
  void clearWriteError();
  
  //-------------------------------------------------------------
  uint8_t close(void);
  uint32_t curPosition(void) const { return (_fl.fs) ? _fl.fptr : 0; }
  uint32_t curSize(void) const { return (_fl.fs) ? _fl.fsize : 0; }

  boolean open(SdFile* dirFile, const char* fileName, uint8_t oflag);
  
  int16_t read(void) {
    uint8_t b;
    return read(&b, 1) == 1 ? b : -1;
  }
  int16_t read(void* buf, uint16_t nbyte);
  
  boolean seekCur(int32_t pos) {
    if(!_fl.fs) return false;
    return seekSet(_fl.fptr + pos);
  }
  boolean seekEnd(void) { return seekSet(_fl.fsize); }
  boolean seekSet(uint32_t pos) {
    if(!_fl.fs) return 0;
    _result = f_lseek (&_fl, pos);            /* Move file pointer of a file object */
    return FR_OK==_result;
  }

  uint8_t isOpen(void) const { return (_fl.fs != NULL); }

  size_t write(uint8_t b);
  size_t write(const void* buf, uint16_t nbyte);
  size_t write(const char* str);

  /**
   * Use unbuffered reads to access this file.  Used with Wave
   * Shield ISR.  Used with Sd2Card::partialBlockRead() in WaveRP.
   *
   * Not recommended for normal applications.
   */
  uint8_t sync(void);

protected:
  // private data
  uint8_t convertFlag(uint8_t f);
 
};


