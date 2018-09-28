













#pragma once

#include "WString.h" 
#include "Stream.h" 
#include "diskio.h" 
#include "ff.h" 
#include "SdFile.h" 


// use the gnu style oflag in open()
/** open() oflag for reading */
uint8_t const O_READ = 0X01;
/** open() oflag - same as O_READ */
uint8_t const O_RDONLY = O_READ;
/** open() oflag for write */
uint8_t const O_WRITE = 0X02;
/** open() oflag - same as O_WRITE */
uint8_t const O_WRONLY = O_WRITE;
/** open() oflag for reading and writing */
uint8_t const O_RDWR = (O_READ | O_WRITE);
/** open() oflag mask for access modes */
uint8_t const O_ACCMODE = (O_READ | O_WRITE);
/** The file offset shall be set to the end of the file prior to each write. */
uint8_t const O_APPEND = 0X04;
/** synchronous writes - call sync() after each write */
//uint8_t const O_SYNC = 0X08;
/** create the file if nonexistent */
uint8_t const O_CREAT = 0x04;
/** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
//uint8_t const O_EXCL = 0X20;
/** truncate the file to zero length */
//uint8_t const O_TRUNC = 0X40;

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

class File : public Stream {
 private:
  char _name[MAX_PATH_LEN]; // our name
  SdFile *_file;  // underlying file pointer

public:
  File(SdFile& f, const char *name);     // wraps an underlying SdFile
  File(void);      // 'empty' constructor
  virtual ~File();
  
  virtual size_t write(uint8_t);
  virtual size_t write(uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();   //OK
  virtual size_t available();   //OK
  virtual void flush(); //OK
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();  //OK
  uint32_t size();      //OK
  void close();         //OK
  operator bool();
  char * name();
  
  boolean isDirectory(void);

  File openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);
  
  using Print::write;
};

class SDClass {
private:
  // These are required for initialisation and use of sdfatlib
  bool fsInit;
  FATFS fatFs;  
  //Sd2Card card;
  //SdVolume volume;
  //SdFile root;
  
  // my quick&dirty iterator, should be replaced
  //SdFile& getParentDir(const char *filepath, int *indx);
  
public:
  SDClass();
  virtual ~SDClass();
  
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN);

  // Methods to determine if the requested file path exists.
  boolean exists(const char *filepath);
  boolean exists(const String &filepath) { return exists(filepath.c_str()); }

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean mkdir(const char *filepath);
  boolean mkdir(const String &filepath) { return mkdir(filepath.c_str()); }
 
  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  File open(const char *filename, uint8_t mode = FILE_READ);
  File open(const String &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

  
  // Delete the file.
  boolean remove(char *filepath);
  boolean remove(const String &filepath) { return remove(filepath.c_str()); }
  
  boolean rmdir(char *filepath);
  boolean rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

  //Not Arduino like interface
  FRESULT result;
  
private:
  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  int fileOpenMode;
  
  friend class File;
  //friend boolean callback_openPath(SdFile&, char *, boolean, void *); 
};

extern SDClass SD;

