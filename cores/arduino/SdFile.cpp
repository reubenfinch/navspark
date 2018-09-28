#include "SdFile.h" 
#include "ff.h" 
#include "SD.h" 

SdFile::SdFile() {
  memset(&_fl, 0, sizeof(_fl));
  memset(&_di, 0, sizeof(_di));
  memset(&_fi, 0, sizeof(_fi));

  _isDir = true;
  _result = FR_OK;
}

SdFile::SdFile(const SdFile& f) {
  memcpy(&_fl, &(f._fl), sizeof(_fl));
  memcpy(&_di, &(f._di), sizeof(_di));
  memcpy(&_fi, &(f._fi), sizeof(_fi));
  memcpy(_name, &(f._name), sizeof(_name));
 
  _isDir = f._isDir;
  _result = f._result;
}

int16_t SdFile::read(void* buf, uint16_t nbyte)
{
  if(!_fl.fs) 
    return -1;
    
  UINT len = nbyte;
  _result = f_read(&_fl, buf, len, &len);
  return (FR_OK==_result) ? (int16_t)len : -1;
}

bool SdFile::isDir() { 
  return _isDir; 
}

FRESULT SdFile::getWriteError() { 
  return _result; 
}

void SdFile::clearWriteError() { 
  _result = FR_OK; 
}

uint8_t SdFile::close(void) {
  _result = f_close(&_fl);
  memset(&_fl, 0, sizeof(_fl));
  return _result == FR_OK;
}

//------------------------------------------------------------------------------
/**
 * Open a file or directory by name.
 *
 * \param[in] dirFile An open SdFat instance for the directory containing the
 * file to be opened.
 *
 * \param[in] fileName A valid 8.3 DOS name for a file to be opened.
 *
 * \param[in] oflag Values for \a oflag are constructed by a bitwise-inclusive
 * OR of flags from the following list
 *
 * O_READ - Open for reading.
 *
 * O_RDONLY - Same as O_READ.
 *
 * O_WRITE - Open for writing.
 *
 * O_WRONLY - Same as O_WRITE.
 *
 * O_RDWR - Open for reading and writing.
 *
 * O_APPEND - If set, the file offset shall be set to the end of the
 * file prior to each write.
 *
 * O_CREAT - If the file exists, this flag has no effect except as noted
 * under O_EXCL below. Otherwise, the file shall be created
 *
 * O_EXCL - If O_CREAT and O_EXCL are set, open() shall fail if the file exists.
 *
 * O_SYNC - Call sync() after each write.  This flag should not be used with
 * write(uint8_t), write_P(PGM_P), writeln_P(PGM_P), or the Arduino Print class.
 * These functions do character at a time writes so sync() will be called
 * after each byte.
 *
 * O_TRUNC - If the file exists and is a regular file, and the file is
 * successfully opened and is not read only, its length shall be truncated to 0.
 *
 * \note Directory files must be opened read only.  Write and truncation is
 * not allowed for directory files.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include this SdFile is already open, \a difFile is not
 * a directory, \a fileName is invalid, the file does not exist
 * or can't be opened in the access mode specified by oflag.
 */
boolean SdFile::open(SdFile* dirFile, const char* fileName, uint8_t oflag) {
  _result = f_open(&_fl, fileName, convertFlag(oflag));
  if(FR_EXIST == _result && (oflag | O_CREAT)) {
    _result = f_open(&_fl, fileName, convertFlag(oflag & ~O_CREAT));
  }
  if(_result != FR_OK) {
    return false;
  }

  f_stat(fileName, &_fi);
  if(oflag & O_WRITE) {
    f_lseek(&_fl, _fi.fsize);
  }
  else
  {
    f_lseek(&_fl, 0);
  }
  return _result==FR_OK;
}

//------------------------------------------------------------------------------
/**
 * Write data to an open file.
 *
 * \note Data is moved to the cache but may not be written to the
 * storage device until sync() is called.
 *
 * \param[in] buf Pointer to the location of the data to be written.
 *
 * \param[in] nbyte Number of bytes to write.
 *
 * \return For success write() returns the number of bytes written, always
 * \a nbyte.  If an error occurs, write() returns -1.  Possible errors
 * include write() is called before a file has been opened, write is called
 * for a read-only file, device is full, a corrupt file system or an I/O error.
 *
 */
size_t SdFile::write(const void* buf, uint16_t nbyte) {
  UINT t = nbyte;
  _result = f_write (&_fl, buf, t, &t); /* Write data to a file */
 
  if(_result == FR_OK)
    return t;
  else
    return 0;
}

//------------------------------------------------------------------------------
/**
 * Write a byte to a file. Required by the Arduino Print class.
 *
 * Use SdFile::writeError to check for errors.
 */
size_t SdFile::write(uint8_t b) {
//strcat(dbg_buf, "G2-");
  return write(&b, 1);
}

//------------------------------------------------------------------------------
/**
 * Write a string to a file. Used by the Arduino Print class.
 *
 * Use SdFile::writeError to check for errors.
 */
size_t SdFile::write(const char* str) {
  return write(str, strlen(str));
}

//------------------------------------------------------------------------------
/**
 * The sync() call causes all modified data and directory fields
 * to be written to the storage device.
 *
 * \return The value one, true, is returned for success and
 * the value zero, false, is returned for failure.
 * Reasons for failure include a call to sync() before a file has been
 * opened or an I/O error.
 */
uint8_t SdFile::sync(void) {
  if(_fl.fs) 
    f_sync(&_fl);
  return 0;
}

uint8_t SdFile::convertFlag(uint8_t f)
{
  uint8_t flag = 0;
  if(f & O_READ) flag |= FA_READ;
  if(f & O_WRITE) flag |= FA_WRITE;
  if(f & O_CREAT) flag |= FA_CREATE_NEW;

//sprintf(dbg_buf, "%X", flag);
  return flag;
}

