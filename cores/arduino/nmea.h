#ifndef _NMEA_PARSER_HEADER_
#define _NMEA_PARSER_HEADER_

#define InvalidateSnr     -1  
#define MaxSatellite      32
#define GsaMaxSatellite   12

typedef signed   char             S08;
typedef unsigned char             U08;
typedef signed   short int        S16;
typedef unsigned short int        U16;
typedef signed   long int         S32;
typedef unsigned long int         U32;
typedef float                     F32;
typedef double                    D64;
typedef const char *LPCSTR;

typedef struct Satellite
{ 
  U16     satelliteId;
  U16     elevation;
  U16     azimuth;
  U16     snr;
} Satellite;

typedef enum NmeaType
{
  MSG_UNKNOWN = 0,  //No complete line
  MSG_GPGSA,
  MSG_GLGSA,
  MSG_BDGSA,
  MSG_GNGSA,
  MSG_GPGSV,
  MSG_GLGSV,
  MSG_BDGSV,
  MSG_GGA,
  MSG_GNS,
  MSG_VTG,
  MSG_GLL,
  MSG_RMC,
  MSG_ZDA,
  MSG_ERROR,  //Format error
  MSG_FULL,   //buffer full, it will discard old data.
} NmeaType;

typedef struct GgaInfo
{
  U16     hour;
  U16     minute;
  F32     second;
  D64     latitude;
  U08     latitudeNS;
  D64     longitude;
  U08     longitudeEW;
  U08     gpsQualityIndicator;
  U16     numsOfSatellites;
  F32     hdop;
  F32     altitude;
  U08     altitudeMeters;
  F32     geoidalSeparation;
  U08     geoidalSeparationMeters;
  F32     ageDGPSData;
  U16     diffRefStaID;
} GgaInfo;

typedef struct GsvInfo
{ 
  U16     numOfMessage;
  U16     sequenceNum;
  U16     numOfSate;
  Satellite    sates[4];
} GsvInfo;

typedef struct GsaInfo
{
  U08     autoMode; //A=Automatic, M=Manual
  U16     fixMode;  //1=fix not available, 2=2D, 3=3D
  U16     sateId[MaxSatellite];
  F32     pdop;
  F32     hdop;
  F32     vdop;
} GsaInfo;

typedef struct RmcInfo
{ 
  U16     hour;
  U16     minute;
  F32     second;
  U08     status;
  D64     latitude;
  U08     latitudeNS;
  D64     longitude;
  U08     longitudeEW;
  F32     speedKnots;
  F32     trueCourse;
  U16     year;
  U16     month;
  U16     day;
  F32     magVar;
  U08     magVarDir;
  U08     modeIndicator;
} RmcInfo;

typedef struct ZdaInfo
{ 
  U16     hour;
  U16     minute;
  F32     second;
  U16     day;
  U16     month;
  U16     year;
  U16     localZoneHours;
  U16     locaZoneMinutes;
} ZdaInfo;

typedef struct VtgInfo
{ 
  F32     trueCourse;//true course over ground in degrees(000.0 ~ 359.9)  
  F32     magneticCourse;//Magnetic course over ground in degrees(000.0 ~ 359.9)
  F32     speedKnots;
  F32     speedKmPerHur;
  U08     mode; //N = not valid, A = Auto, D = Diff, E = Estimated, M = Manual, S = Simulator
} VtgInfo;

typedef struct GllInfo
{
  D64     latitude;
  U08     latitudeNS;
  D64     longitude;
  U08     longitudeEW;
  U16     hour;
  U16     minute;
  F32     second;
  U08     status;   //A = Data valid, V = Data not valid
  U08     modeIndicator;
} GllInfo;

//Get nmea type and check its validation.
NmeaType MessageType(LPCSTR pt, int len);

#define InBufferSize    2 * 1024
#define LineBufferSize  256
#define MaxNmeaParam 32

class NmeaParser
{
public:
  NmeaParser();

  /* buf - NMEA data block, size - data size(not buffer size)
     return - NMEA type */
  NmeaType NmeaDataIn(U08* buf, int size);
 
  const GgaInfo* GetGGA() const { return &gga; }
  const GgaInfo* GetGNS() const { return &gns; }
  const GsvInfo* GetGSV() const { return &gsv; }
  const GsaInfo* GetGSA() const { return &gsa; }
  const RmcInfo* GetRMC() const { return &rmc; }
  const ZdaInfo* GetZDA() const { return &zda; }
  const VtgInfo* GetVTG() const { return &vtg; }
  const GllInfo* GetGLL() const { return &gll; }
  
private:
  RmcInfo rmcInfo;
  U08 inBuffer[InBufferSize];
  int inBufferLen;
  int inBufferPtr;
  U08 lineBuffer[LineBufferSize];
  int lineLen;

  GgaInfo gga;
  GgaInfo gns;
  GsvInfo gsv;
  GsaInfo gsa;
  RmcInfo rmc;
  ZdaInfo zda;
  VtgInfo vtg;
  GllInfo gll;  
  
  int GGAProc(GgaInfo* rgga, U08* pt, int len);
  int GNSProc(GgaInfo* rgga, U08* pt, int len);
  int GSVProc(GsvInfo* rgsv, U08* pt, int len);
  int GSAProc(GsaInfo* rgsa, U08* pt, int len);
  int RMCProc(RmcInfo* rrmc, U08* pt, int len);
  int ZDAProc(ZdaInfo* rzda, U08* pt, int len);
  int VTGProc(VtgInfo* rvtg, U08* pt, int len);
  int GLLProc(GllInfo* rgll, U08* pt, int len);  

  void ClearInBuffer();
  void OrganizeInBuffer();
  void ClearLineBuffer();
  int ReadOneLine();
  NmeaType MessageType(LPCSTR pt, int len);
  int VarifyNmeaChecksum(LPCSTR pt, int len);
  int LSB(char lsb);
  int MSB(char msb);
  int ParamInt(U08* p, int first, int second, int defaultValue);
  char ParamChar(U08* p, int first, int second, char defaultValue);
  float ParamFloat(U08* p, int first, int second, float defaultValue);
  double ParamDouble(U08* p, int first, int second, float defaultValue);
};

#endif  //end of #ifndef _NMEA_PARSER_HEADER_
