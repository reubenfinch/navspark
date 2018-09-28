#include <nmea.h>
#include <string>
#include <sti_gnss_lib.h>

NmeaParser::NmeaParser()
{
  memset(&rmcInfo, 0, sizeof(rmcInfo));
  ClearInBuffer();
  ClearLineBuffer();

  memset(&gga, 0, sizeof(gga));
  memset(&gns, 0, sizeof(gns));
  memset(&gsv, 0, sizeof(gsv));
  memset(&gsa, 0, sizeof(gsa));
  memset(&rmc, 0, sizeof(rmc));
  memset(&zda, 0, sizeof(zda));
  memset(&vtg, 0, sizeof(vtg));
  memset(&gll, 0, sizeof(gll));
}

NmeaType NmeaParser::NmeaDataIn(U08* buf, int size)
{
  if(inBufferLen + size > InBufferSize)
  { //Buffer full, clear burrer and doesn't parse.
    ClearInBuffer();
    memcpy(inBuffer, buf, size);
    inBufferLen = size;
    return MSG_FULL;
  }
  memcpy(inBuffer + inBufferLen, buf, size);
  inBufferLen += size;
  int r = ReadOneLine();
  if(r==0)
  {
    return MSG_UNKNOWN;
  }
  
  lineLen = r;
  int pret = 0;
  NmeaType t = MessageType((const char*)lineBuffer, lineLen);

  switch(t)
  {
    case MSG_UNKNOWN:
      //printf("Unknown nmea string!\r\n");
      break;
    case MSG_GPGSA:
    case MSG_GLGSA:
    case MSG_BDGSA:
    case MSG_GNGSA:
      memset(&gga, 0, sizeof(gga));
      pret = GSAProc(&gsa, lineBuffer, lineLen);
      break;
    case MSG_GPGSV:
    case MSG_GLGSV:
    case MSG_BDGSV:
      memset(&gsv, 0, sizeof(gsv));
      pret = GSVProc(&gsv, lineBuffer, lineLen);
      break;
    case MSG_GGA:
      memset(&gga, 0, sizeof(gga));
      pret = GGAProc(&gga, lineBuffer, lineLen);
      break;
    case MSG_GNS:
      memset(&gns, 0, sizeof(gns));
      pret = GNSProc(&gns, lineBuffer, lineLen);
      break;
    case MSG_VTG:
      memset(&vtg, 0, sizeof(vtg));
      pret = VTGProc(&vtg, lineBuffer, lineLen);
      break;
    case MSG_GLL:
      memset(&gll, 0, sizeof(gll));
      pret = GLLProc(&gll, lineBuffer, lineLen);
      break;
    case MSG_RMC:
      memset(&rmc, 0, sizeof(rmc));
      pret = RMCProc(&rmc, lineBuffer, lineLen);
      break;
    case MSG_ZDA:
      memset(&zda, 0, sizeof(zda));
      pret = ZDAProc(&zda, lineBuffer, lineLen);
      break;
    case MSG_ERROR:
      //printf("Error nmea string!\r\n");
      break;
    default:
      break;
  }
  return t;
}

void NmeaParser::ClearInBuffer()
{
  memset(inBuffer, 0, sizeof(inBuffer));
  inBufferLen = 0;
  inBufferPtr = 0;
}
 
void NmeaParser::OrganizeInBuffer()
{
  if(inBufferPtr==0)
    return;
  if(inBufferPtr == inBufferLen)
  {
    ClearInBuffer();
    return;
  }

  int trueLen = inBufferLen-inBufferPtr;
  for(int i=0; i<trueLen; ++i)
  {
    inBuffer[i] = inBuffer[inBufferPtr];
    ++inBufferPtr;
  }
  inBufferLen = trueLen;
  inBufferPtr = 0;
}

void NmeaParser::ClearLineBuffer()
{
  memset(lineBuffer, 0, sizeof(lineBuffer));
  lineLen = 0;
}
                                        //lineBuffer, LineBufferSize
int NmeaParser::ReadOneLine(/*HANDLE h, char* buffer, const int bufferSize*/)
{
  U08* bufferIter = lineBuffer;
  int totalSize = 0;
  int bkInPtr = inBufferPtr;
  if(inBufferLen == 0)
  {
    return 0;
  }

  while(totalSize < LineBufferSize - 1)
  { 
    if(inBufferLen == inBufferPtr) 
    { //No new line until inBuffer end, return and restore inBufferPtr
      inBufferPtr = bkInPtr;
      return 0;
    }
    
    int bytesInBuff = inBufferLen - inBufferPtr;
    while(bytesInBuff)
    {
      *bufferIter = inBuffer[inBufferPtr];
      ++inBufferPtr;
      ++totalSize;
      if(totalSize > 1)
      { //not first char.
        if(*bufferIter == 0x0A && *(bufferIter - 1) == 0x0D)
        { //finished read one line
          OrganizeInBuffer();
          return totalSize - 2; //return length without 0x0d,0x0a
        }
      }

      if (totalSize ==  LineBufferSize - 1)
      { //Line buffer full
        *(bufferIter+1) = 0;
        OrganizeInBuffer();
        return totalSize;
      }
      ++bufferIter;
      --bytesInBuff;
    } //while(bytesinbuff)
  }
  return 0;
}

NmeaType NmeaParser::MessageType(LPCSTR pt, int len)
{
  typedef struct NmeaTypeEntry
  {
    LPCSTR subNmea;
    NmeaType type;
  } NmeaTypeEntry;

  NmeaTypeEntry nmeaTable[] = {
    { "$GPGSA,", MSG_GPGSA },
    { "$GLGSA,", MSG_GLGSA },
    { "$BDGSA,", MSG_BDGSA },
    { "$GNGSA,", MSG_GNGSA },

    { "$GPGSV,", MSG_GPGSV },
    { "$GLGSV,", MSG_GLGSV },
    { "$BDGSV,", MSG_BDGSV },

    { "$GPGGA,", MSG_GGA },
    { "$GLGGA,", MSG_GGA },
    { "$BDGGA,", MSG_GGA },
    { "$GNGGA,", MSG_GGA },

    { "$GPGNS,", MSG_GNS },
    { "$GLGNS,", MSG_GNS },
    { "$BDGNS,", MSG_GNS },
    { "$GNGNS,", MSG_GNS },

    { "$GPVTG,", MSG_VTG },
    { "$GLVTG,", MSG_VTG },
    { "$BDVTG,", MSG_VTG },
    { "$GNVTG,", MSG_VTG },

    { "$GPGLL,", MSG_GLL },
    { "$GLGLL,", MSG_GLL },
    { "$BDGLL,", MSG_GLL },
    { "$GNGLL,", MSG_GLL },

    { "$GPRMC,", MSG_RMC },
    { "$GLRMC,", MSG_RMC },
    { "$BDRMC,", MSG_RMC },
    { "$GNRMC,", MSG_RMC },

    { "$GPZDA,", MSG_ZDA },
    { "$GLZDA,", MSG_ZDA },
    { "$BDZDA,", MSG_ZDA },
    { "$GNZDA,", MSG_ZDA },

    { NULL, MSG_UNKNOWN }
  };

  int i = 0;
  NmeaType returnType = MSG_UNKNOWN;
  while(nmeaTable[i].type)
  {
    if(0==strncmp(pt, nmeaTable[i].subNmea, strlen(nmeaTable[i].subNmea)))
    {
      returnType = nmeaTable[i].type;
      break;
    }
    ++i;
  }

  if(MSG_UNKNOWN==returnType)
  {
    return returnType;
  }

  if(!VarifyNmeaChecksum(pt, len))
  {
    return MSG_ERROR;
  }
  return returnType;
}

int NmeaParser::VarifyNmeaChecksum(LPCSTR pt, int len)
{
  int i = 0;
  char checksum = 0;  

  for(i=1; i<len-3; ++i)
  {   
    checksum ^= pt[i];
  } 
  return checksum == (char)(MSB(*(pt+len-2)) + LSB(*(pt+len-1)));
}

int NmeaParser::LSB(char lsb)
{
  if(lsb >= '0' && lsb <= '9')
  {
    return (lsb - '0');
  }
  else if(lsb >= 'A' && lsb <= 'F')
  {
    return (lsb - 'A' + 0xA);
  }
  return 0;
}

int NmeaParser::MSB(char msb)
{
  return LSB(msb) * 0x10;
}

int NmeaParser::ParamInt(U08* p, int first, int second, int defaultValue)
{
  //ASSERT(first <= second - 1);
  switch(second - first - 1)
  {
  case 0:
    return defaultValue;
  case 1:
    return p[first + 1] - '0';
    break;
  case 2:
    return (p[first + 1] - '0') * 10 + (p[first + 2] - '0');
    break;
  case 3:
    return (p[first + 1] - '0') * 100 + (p[first + 2] - '0') * 10 + 
      (p[first + 3] - '0');
    break;
  case 4:
    return (p[first + 1] - '0') * 1000 + (p[first + 2] - '0') * 100 + 
      (p[first + 3] - '0') * 10 + (p[first + 4] - '0');
  case 5:
    return (p[first + 1] - '0') * 10000 + (p[first + 2] - '0') * 1000 + 
      (p[first + 3] - '0') * 100 + (p[first + 4] - '0') * 10 + 
      (p[first + 5] - '0');
    break;
  case 6:
    return (p[first + 1] - '0') * 100000 + (p[first + 2] - '0') * 10000 + 
      (p[first + 3] - '0') * 1000 + (p[first + 4] - '0') * 100 + 
      (p[first + 5] - '0') * 10 + (p[first + 6] - '0');
    break;
  case 7:
    return (p[first + 1] - '0') * 1000000 + (p[first + 2] - '0') * 100000 + 
      (p[first + 3] - '0') * 10000 + (p[first + 4] - '0') * 1000 + 
      (p[first + 5] - '0') * 100 + (p[first + 6] - '0') * 10 + 
      (p[first + 7] - '0');
    break;
  default:
    //ASSERT(FALSE);
    break;
  }
  return 0;
}

char NmeaParser::ParamChar(U08* p, int first, int second, char defaultValue)
{
  //ASSERT(first == second - 2 || first == second - 1);
  switch(second - first - 1)
  {
  case 0:
    return defaultValue;
  case 1:
    return p[first + 1];;
    break;
  default:
    //ASSERT(FALSE);
    break;
  }
  return 0;
}

float NmeaParser::ParamFloat(U08* p, int first, int second, float defaultValue)
{
  const float scaleTable[] = { 1.0E-1F, 1.0E-2F, 1.0E-3F, 1.0E-4F, 1.0E-5F, 1.0E-6F, 1.0E-7F};
  int dotPos = 0;
  int i = 0;

  if(second - first == 1)
  {
    return defaultValue;
  }
  for(i=first+1; i<second-1; ++i)
  {
    if(p[i]=='.')
    {
      dotPos = i;
      break;
    }
  }
  //ASSERT(dotPos);
  return ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2]  + ParamInt(p, first, dotPos, 0);
}

double NmeaParser::ParamDouble(U08* p, int first, int second, float defaultValue)
{
  const double scaleTable[] = { 1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7};
  int dotPos = 0;
  int i = 0;

  if(second - first == 1)
  {
    return defaultValue;
  }
  for(i=first+1; i<second-1; ++i)
  {
    if(p[i]=='.')
    {
      dotPos = i;
      break;
    }
  }
  return ParamInt(p, dotPos, second, 0) * scaleTable[second-dotPos-2]  + ParamInt(p, first, dotPos, 0);
}

static int ScanDot(U08* pt, int len, int dot[MaxNmeaParam])
{
  int dotPos = 0;
  int i = 0;
  //Pass the beginning "$GPGSV" and the ending "*nn"
  for(i=6; i<len-3; ++i)
  {
    if(pt[i] == ',')
    {
      dot[dotPos] = i;
      ++dotPos;
    }
  }
  dot[dotPos] = len - 3;
  return dotPos;
}

int NmeaParser::GGAProc(GgaInfo* rgga, U08* pt, int len)
{  
  int dot[MaxNmeaParam] = { 0 };      
  ScanDot(pt, len, dot);
  if(dot[0] != 6 || pt[len - 3] != '*')
  {
    return 0;
  }

  rgga->hour = ParamInt(pt, dot[0], dot[0]+3, 0);
  rgga->minute = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
  rgga->second = ParamFloat(pt, dot[0]+4, dot[1], 0);
  rgga->latitude = ParamDouble(pt, dot[1], dot[2], 0.0F);
  rgga->latitudeNS = (U08)ParamChar(pt, dot[2], dot[3], 0);
  rgga->longitude = ParamDouble(pt, dot[3], dot[4], 0.0F);
  rgga->longitudeEW = (U08)ParamChar(pt, dot[4], dot[5], 0);
  rgga->gpsQualityIndicator = (U08)ParamChar(pt, dot[5], dot[6], 0);
  rgga->numsOfSatellites = (U08)ParamInt(pt, dot[6], dot[7], 0);
  rgga->hdop = ParamFloat(pt, dot[7], dot[8], 0);
  if(dot[9])
  { //Some GGA doesn't have those data.
    rgga->altitude = ParamFloat(pt, dot[8], dot[9], 0);
    rgga->altitudeMeters = (U08)ParamChar(pt, dot[9], dot[10], 0);
    rgga->geoidalSeparation = ParamFloat(pt, dot[10], dot[11], 0);
    rgga->geoidalSeparationMeters = (U08)ParamChar(pt, dot[11], dot[12], 0);
    rgga->ageDGPSData = ParamFloat(pt, dot[12], dot[13], 0);
    rgga->diffRefStaID = (U16)ParamInt(pt, dot[13], dot[14], 0);
  }
  return 1;
}

int NmeaParser::GNSProc(GgaInfo* rgga, U08* pt, int len)
{ 
  int dot[MaxNmeaParam] = {0};      
  ScanDot(pt, len, dot);
  if(dot[0] != 6 || pt[len - 3] != '*')
  {
    return 0;
  }

  rgga->hour = ParamInt(pt, dot[0], dot[0]+3, 0);
  rgga->minute = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
  rgga->second = ParamFloat(pt, dot[0]+4, dot[1], 0);
  rgga->latitude = ParamDouble(pt, dot[1], dot[2], 0.0F);
  rgga->latitudeNS = (U08)ParamChar(pt, dot[2], dot[3], 0);
  rgga->longitude = ParamDouble(pt, dot[3], dot[4], 0.0F);
  rgga->longitudeEW = (U08)ParamChar(pt, dot[4], dot[5], 0);
  rgga->gpsQualityIndicator = (U16)ParamChar(pt, dot[5], dot[5]+2, 0) << 8;
  rgga->gpsQualityIndicator += (U16)ParamChar(pt, dot[5]+1, dot[6], 0);

  rgga->numsOfSatellites = ParamInt(pt, dot[6], dot[7], 0);
  rgga->hdop = ParamFloat(pt, dot[7], dot[8], 0);
  rgga->altitude = ParamFloat(pt, dot[8], dot[9], 0);
  rgga->geoidalSeparation = ParamFloat(pt, dot[9], dot[10], 0);
  rgga->ageDGPSData = ParamFloat(pt, dot[10], dot[11], 0);
  rgga->diffRefStaID = (U16)ParamInt(pt, dot[11], dot[12], 0);
  return 1;
}

int NmeaParser::GSVProc(GsvInfo* rgsv, U08* pt, int len)
{
  int dot[MaxNmeaParam] = { 0 };      
  int dotPos = ScanDot(pt, len, dot);
  int i=0, groupIdx=0;

  if( (dotPos + 1) != 8 && (dotPos + 1) != 12 && 
    (dotPos + 1) != 16 && (dotPos + 1) != 20 ) 
  {
    return 0;
  }

  if(dot[0] != 6 || pt[len - 3] != '*')
  {
    return 0;
  }

  rgsv->numOfMessage = ParamInt(pt, dot[0], dot[1], 0);
  rgsv->sequenceNum = ParamInt(pt, dot[1], dot[2], 0);
  rgsv->numOfSate = ParamInt(pt, dot[2], dot[3], 0);
    for(i=3, groupIdx=0; i<dotPos; i+=4, ++groupIdx)
  {
    rgsv->sates[groupIdx].satelliteId = ParamInt(pt, dot[i], dot[i+1], 0);
    rgsv->sates[groupIdx].elevation = ParamInt(pt, dot[i+1], dot[i+2], 0);
    rgsv->sates[groupIdx].azimuth = ParamInt(pt, dot[i+2], dot[i+3], 0);
    rgsv->sates[groupIdx].snr = ParamInt(pt, dot[i+3], dot[i+4], InvalidateSnr);
  }
  return 1;
}

int NmeaParser::GSAProc(GsaInfo* rgsa, U08* pt, int len)
{ 
  int dot[MaxNmeaParam] = { 0 };      
  int dotPos = ScanDot(pt, len, dot);
  int idPos = 0;
  int i = 0;

  if(dot[0] != 6 || pt[len - 3] != '*')
  {
    return 0;
  }

  rgsa->autoMode = ParamChar(pt, dot[0], dot[1], 0);
  rgsa->fixMode = ParamInt(pt, dot[1], dot[2], 0);

  memset(rgsa->sateId, 0, sizeof(rgsa->sateId));
  for(i=2; (i<2 + GsaMaxSatellite) && (i+1<=dotPos); ++i)
  {
    rgsa->sateId[idPos] = ParamInt(pt, dot[i], dot[i+1], 0);;
    ++idPos;
  }   

  rgsa->pdop = ParamFloat(pt, dot[14], dot[15], 0.0F);
  rgsa->hdop = ParamFloat(pt, dot[15], dot[16], 0.0F);
  rgsa->vdop = ParamFloat(pt, dot[16], dot[17], 0.0F);
  return 1;
}

int NmeaParser::RMCProc(RmcInfo* rrmc, U08* pt, int len)
{ 
  int dot[MaxNmeaParam] = {0};      
  int dotPos = ScanDot(pt, len, dot);

  if(dot[0] != 6 || pt[len - 3] != '*' || (dotPos+1) != 13)
  {
    return 0;
  }

  rrmc->hour = ParamInt(pt, dot[0], dot[0]+3, 0);
  rrmc->minute = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
  rrmc->second = ParamFloat(pt, dot[0]+4, dot[1], 0.0F);

  rrmc->status = (U08)ParamChar(pt, dot[1], dot[2], 0);
  rrmc->latitude = ParamDouble(pt, dot[2], dot[3], 0.0F);
  rrmc->latitudeNS = (U08)ParamChar(pt, dot[3], dot[4], 0);
  rrmc->longitude = ParamDouble(pt, dot[4], dot[5], 0.0F);
  rrmc->longitudeEW = (U08)ParamChar(pt, dot[5], dot[6], 0);
  rrmc->speedKnots = ParamFloat(pt, dot[6], dot[7], 0.0F);
  rrmc->trueCourse = ParamFloat(pt, dot[7], dot[8], 0.0F);

  rrmc->day = ParamInt(pt, dot[8], dot[8]+3, 0);
  rrmc->month = ParamInt(pt, dot[8]+2, dot[8]+5, 0);
  rrmc->year = ParamInt(pt, dot[8]+4, dot[9], 0) + 2000;
  
  rrmc->magVar = ParamFloat(pt, dot[9], dot[10], 0.0F);
  rrmc->magVarDir = (U08)ParamChar(pt, dot[10], dot[11], 0);
  rrmc->modeIndicator = (U08)ParamChar(pt, dot[11], dot[12], 0);
  return 1;
}

int NmeaParser::ZDAProc(ZdaInfo* rzda, U08* pt, int len)
{ 
  int dot[MaxNmeaParam] = {0};      
  int dotPos = ScanDot(pt, len, dot);
  if(dot[0] != 6 || pt[len - 3] != '*' || (dotPos+1) != 7)
  {
    return 0;
  }

  rzda->hour = ParamInt(pt, dot[0], dot[0]+3, 0);
  rzda->minute = ParamInt(pt, dot[0]+2, dot[0]+5, 0);
  rzda->second = ParamFloat(pt, dot[0]+4, dot[1], 0.0F);

  rzda->day = ParamInt(pt, dot[1], dot[2], 0);
  rzda->month = ParamInt(pt, dot[2], dot[3], 0);
  rzda->year = ParamInt(pt, dot[3], dot[4], 0);
  rzda->localZoneHours = ParamInt(pt, dot[4], dot[5], 0);
  rzda->locaZoneMinutes = ParamInt(pt, dot[5], dot[6], 0);
  return 1;
}

int NmeaParser::VTGProc(VtgInfo* rvtg, U08* pt, int len)
{ 
  int dot[MaxNmeaParam] = {0};      
  ScanDot(pt, len, dot);
  if(dot[0] != 6 || pt[len - 3] != '*')
  {
    return 0;
  }

  rvtg->trueCourse = ParamFloat(pt, dot[0], dot[1], 0.0F);
  rvtg->magneticCourse = ParamFloat(pt, dot[2], dot[3], 0.0F);
  rvtg->speedKnots = ParamFloat(pt, dot[4], dot[5], 0.0F);
  rvtg->speedKmPerHur = ParamFloat(pt, dot[6], dot[7], 0.0F);
  rvtg->mode = (U08)ParamChar(pt, dot[8], dot[9], 0);
  return 1;
}

int NmeaParser::GLLProc(GllInfo* rgll, U08* pt, int len)
{
  int dot[MaxNmeaParam] = {0};      
  int dotPos = ScanDot(pt, len, dot);
  if(dot[0] != 6 || pt[len - 3] != '*' || (dotPos+1) != 8)
  {
    return 0;
  }
  rgll->latitude = ParamDouble(pt, dot[0], dot[1], 0.0F);
  rgll->latitudeNS = (U08)ParamChar(pt, dot[1], dot[2], 0);
  rgll->longitude = ParamDouble(pt, dot[2], dot[3], 0.0F);
  rgll->longitudeEW = (U08)ParamChar(pt, dot[3], dot[4], 0);
  rgll->hour = ParamInt(pt, dot[4], dot[4]+3, 0);
  rgll->minute = ParamInt(pt, dot[4]+2, dot[4]+5, 0);
  rgll->second = ParamFloat(pt, dot[4]+4, dot[5], 0.0F);  
  rgll->status = (U08)ParamChar(pt, dot[5], dot[6], 'A'); 
  rgll->modeIndicator = (U08)ParamChar(pt, dot[6], dot[7], 0);  
  return 1;
}
