#include "SD.h"
#include "SPI.h"
#include "HardwareSerial.h"

extern char dbg_buf[1024];
SDClass SD;
SDClass::SDClass()
{
  fsInit = false;
  result = FR_OK;
  memset(&fatFs, 0, sizeof(fatFs));
}

SDClass::~SDClass()
{
  
}

boolean SDClass::begin(uint8_t csPin) {
  spiMaster.config(0, 10000000, false, false); // mode 0, 10MHz, CS0 and CS1 
  spiMaster.begin(); 
  spiMaster.slaveSelect(0); // use GPIO28 
  
  result = f_mount(0, &fatFs);
  return (FR_OK == result);
}

File SDClass::open(const char *filepath, uint8_t mode) {
  //int pathidx;

  // do the interative search
  //SdFile parentdir = getParentDir(filepath, &pathidx);
  // no more subdirs!

  //filepath += pathidx;

  if (0 == filepath[0]) {
    // it was the directory itself!
    return File();
  }
  
  SdFile file;
  if(!file.open(NULL, filepath, mode)) {
    return File();
  }

  // Open the file itself
  return File(file, filepath);
}

// this little helper is used to traverse paths
/*
SdFile& SDClass::getParentDir(const char *filepath, int *index) {
  // get parent directory
  SdFile d1 = root; // start with the mostparent, root!
  SdFile d2;

  // we'll use the pointers to swap between the two objects
  SdFile *parent = &d1;
  SdFile *subdir = &d2;
  
  const char *origpath = filepath;

  while (strchr(filepath, '/')) {

    // get rid of leading /'s
    if (filepath[0] == '/') {
      filepath++;
      continue;
    }
    
    if (! strchr(filepath, '/')) {
      // it was in the root directory, so leave now
      break;
    }

    // extract just the name of the next subdirectory
    uint8_t idx = strchr(filepath, '/') - filepath;
    if (idx > 12)
      idx = 12;    // dont let them specify long names
    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    // close the subdir (we reuse them) if open
    subdir->close();
    if (! subdir->open(parent, subdirname, O_READ)) {
      // failed to open one of the subdirectories
      SdFile tmp =  SdFile();
      return tmp;
    }
    // move forward to the next subdirectory
    filepath += idx;

    // we reuse the objects, close it.
    parent->close();

    // swap the pointers
    SdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  // parent is now the parent diretory of the file!
  return *parent;
}
*/

/*
 Returns true if the supplied file path exists.
*/
boolean SDClass::exists(const char *filepath) {
  DIR d = {0};
  FIL f = {0};

  FRESULT fr = f_opendir(&d, filepath);
  if(fr == FR_OK)
    return true;

  fr = f_open(&f, filepath, FA_READ); 
  if(fr == FR_OK)
  {
   f_close(&f); 
    return true;
  }
  return false;
}

/*
  Makes a single directory or a heirarchy of directories.
  A rough equivalent to `mkdir -p`.
*/
boolean SDClass::mkdir(const char *filepath) {
  FRESULT res = f_mkdir(filepath);
  return FR_OK == res;   
}

boolean SDClass::remove(char *filepath) {
  FRESULT res = f_unlink (filepath);
  return FR_OK == res;   
}

boolean SDClass::rmdir(char *filepath) {
  FRESULT res = f_unlink (filepath);
  return FR_OK == res;   
}



