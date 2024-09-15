
#include <cstdio>
#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <iomanip>

#include "compiler.hpp"

#define CO_DEBUG if (m_dbg) CERR
#define NEXT_REG(i) \
	"REG" << std::dec << std::setw(2) << std::setfill('0') << (i / 16) << "  "

Compiler* Compiler::m_pInstance = 0;

const char* Compiler::m_stampID[] = {
  STAMPID_NONE,
  STAMPID_RESERVED,
  STAMPID_BS2,
  STAMPID_BS2E,
  STAMPID_BS2SX,
  STAMPID_BS2P,
  STAMPID_BS2PE,
  STAMPID_BS2PX,
  0
};

//------------------------------------------------------------------------------
// Compiler
//------------------------------------------------------------------------------
Compiler::Compiler():
  m_pTk(0),
  m_index(0),
  m_dbg(false),
  m_pRec(0)
{
}

//------------------------------------------------------------------------------
// ~Compiler
//------------------------------------------------------------------------------
Compiler::~Compiler()
{
  if (m_pRec) {
    delete m_pRec;
    m_pRec = 0;
  }
}

//------------------------------------------------------------------------------
// instance
//------------------------------------------------------------------------------
Compiler* Compiler::instance()
{
  if (!m_pInstance) {
    m_pInstance = new Compiler;
  }

  return m_pInstance;
}

//------------------------------------------------------------------------------
// free
//------------------------------------------------------------------------------
void Compiler::free()
{
  delete m_pInstance;
  m_pInstance = 0;
}

//------------------------------------------------------------------------------
// testRecAlignment
//------------------------------------------------------------------------------
bool Compiler::testRecAlignment()
{
  TModuleRec rec;
  return m_pTk->TestRecAlignment(&rec);
}

//------------------------------------------------------------------------------
// printVersionInfo
//------------------------------------------------------------------------------
void Compiler::printVersionInfo()
{
  int ver = m_pTk->Version();
  CERR << "compiler using PARALLAX tokenizer library " <<
    ver / 100 << "." <<
    ver % 100 << std::endl;
}

//------------------------------------------------------------------------------
// compile
//------------------------------------------------------------------------------
bool Compiler::compile(
  const std::string& srcFile,
  const std::string& objFile,
  std::string& sType,
  std::string& sPort,
  bool directivesOnly, 
  bool parseStampDirective,
  bool isProject)
{
  if (m_pRec) {
    delete m_pRec;
    m_pRec = 0;
  }
  m_pRec = new TModuleRec;

  bool bCompiled(false);

  CERR << "compiling project file " << srcFile << std::endl;

  if (isProject) {
    m_index = 0;
  }

  // v1.16
  char* pSource = new char[65536];

  // v1.16
  int srcSize = readSourceFile(srcFile, pSource);

  if (srcSize) {
    // if source available, try compile
    m_pRec->SourceSize = srcSize;

    m_pRec->TargetModule = stampIDToModuleID(sType);

    // v.1.16
    bCompiled = m_pTk->Compile(m_pRec, pSource, directivesOnly,
      parseStampDirective, 0);

    if (bCompiled) {
      CO_DEBUG << EYEINF << " compiled " << srcFile << ENDL;

      // values from compiler directives
      sType = moduleIDToStampID(m_pRec->TargetModule);
      sPort = m_pRec->Port ? std::string(m_pRec->Port) : "";

      // Note: the port returned by the tokenizer (compile) is given
      // in Windows-format (COM1, COM2...) and has to be converted
      // to a unix device name.
      if (sPort != "") {
        int iPortNr = atoi(sPort.substr(3).c_str()) - 1;
        char rcPortNr[2];
        sprintf(rcPortNr, "%1u", iPortNr);
        sPort = std::string(COMM_DEVICE) + rcPortNr;
      }

      CO_DEBUG << EYEINF << " {$STAMP} " << sType << ENDL;
      CO_DEBUG << EYEINF << " {$PORT} " << sPort << ENDL;

      // write object file
      // no objFile for syntax checking only
      if (objFile != "") {
        int objSize;
        objSize = writeObjectFile(objFile, m_pRec->PacketBuffer,
          m_pRec->PacketCount);
      }

      if (isProject) {
        // get the names of the other source files of this project
        for (int i = 0; i < 6 && m_pRec->ProjectFiles[i]; i++) {
          m_sSource[i] = m_pRec->ProjectFiles[i];
        }
      }
    }

    else {
      // did not compile
      CERR << EYEERR << " " << m_pRec->Error << " in " << srcFile << ENDL;
      printOffendingLine(m_pRec, pSource);
    }
  }

  delete pSource;

  return bCompiled;
}

//------------------------------------------------------------------------------
// dump
//------------------------------------------------------------------------------
void Compiler::dump(bool bMap)
{
  if (m_pRec) {
    dumpEtc();

    CERR << "memory usage" << std::endl << std::endl;

    if (bMap) {
      dumpEEPROMLayout();
      dumpEEPROM(); 
    }

    dumpVariables(); 
    showPcntUsage();
  }
  else {
    CERR << EYEERR << " nothing to dump" << ENDL;
  }
}

//------------------------------------------------------------------------------
// getNextSource
//------------------------------------------------------------------------------
std::string Compiler::getNextSource()
{
  return m_sSource[m_index++];
}

//------------------------------------------------------------------------------
// setDebug
//------------------------------------------------------------------------------
void Compiler::setDebug(bool b) { m_dbg = b; }

//------------------------------------------------------------------------------
// moduleIDToStampID
//------------------------------------------------------------------------------
std::string Compiler::moduleIDToStampID(int mid)
{
  return std::string(m_stampID[mid]);
}

//------------------------------------------------------------------------------
// stampIDToModuleID
//------------------------------------------------------------------------------
int Compiler::stampIDToModuleID(std::string sid)
{
  // PRELIMINARY support of the BS2px: compile code for the BS2p
  #if FOREIGN_STAMP == 1
  if (sid == STAMPID_FOREIGN) {
    CERR << EYEINF << " WARNING, preliminary " << STAMPID_FOREIGN <<
      " support:" << ENDL;
    CERR << EYEINC << " tokenizing for " << STAMPID_FOREIGNTARGET <<
      " instead" << ENDL;
    return stampIDToModuleID(STAMPID_FOREIGNTARGET);
  }
  #endif

  int i(0);
  while (m_stampID[i] && strcasecmp(m_stampID[i], sid.c_str()) != 0) {
    i++;
  }

  return m_stampID[i] ? i : 0;
}

//------------------------------------------------------------------------------
// readSourceFile
//------------------------------------------------------------------------------
int Compiler::readSourceFile(const std::string& srcFile, char* srcBuffer)
{
  FILE *fp;
  int srcSize(0);

  fp = fopen(srcFile.c_str(), "r");

  if (fp) {
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
      *srcBuffer++ = ch;
      srcSize++;
    }

    fclose(fp);

    CO_DEBUG << EYEINF << " source file read: " << srcFile << " (" << 
      srcSize << " byte)" << ENDL;
  }

  else {
    CERR << EYEERR << " " << strerror(errno) << ": " << srcFile << ENDL;
  }

  return srcSize;
}

//------------------------------------------------------------------------------
// writeObjectFile
//------------------------------------------------------------------------------
int Compiler::writeObjectFile(const std::string& objFile, byte* pckBuffer, int pckCount)
{
  FILE *fp;
  int objSize(0);

  fp = fopen(objFile.c_str(), "w");

  if (fp) {
    for (int i = 0; i < (PACKETSIZE * pckCount); i++) {
      putc(*pckBuffer++, fp);
      objSize++;
    }

    fclose(fp);

    CO_DEBUG << EYEINF << " object file written: " << objFile << " (" << 
      objSize << " byte)" << ENDL;
  }

  else {
    CERR << EYEERR << " " << strerror(errno) << ": " << objFile << ENDL;
  }

  return objSize;
}

//------------------------------------------------------------------------------
// printOffendningLine
//------------------------------------------------------------------------------
void Compiler::printOffendingLine(TModuleRec *rec, 
  char* src)
{
  // nothing to print
  if (rec->ErrorLength == 0) return;

  const char NLCHR = '\003';

  // v1.16
  int srcStart   = rec->ErrorStart;
  int srcStart0  = srcStart;
  // v1.16
  int srcLength  = rec->ErrorLength;
  int srcLength0 = srcLength;
  int srcSize    = rec->SourceSize;
  int srcLine(1);

  // count lines up to srcStart
  for (int k = 0; k < srcStart; k++) {
    if (src[k] == NLCHR) srcLine++;
  }

  // expand right to next NLCHR
  for (int i = srcStart + srcLength; 
    i < srcSize - 1 && src[i] != NLCHR; 
    i++, srcLength++
  );

  // expand left to preceeding NLCHR
  for (;
    srcStart > 0 && src[srcStart - 1] != NLCHR; 
    srcStart--, srcLength++
  );

  std::string sLine(&src[srcStart], srcLength);
  std::string sUline;
  sUline.assign(srcLength0, '^');
  int prfxLen = srcStart0 - srcStart;

  // print offending line and mark error
  CERR << EYEERC << " " << srcLine << ": " << sLine << ENDL;
  CERR << EYEERC << " " << srcLine << ": ";
  // print a prefix of the right number of tabs and blanks
  for (int i = 0; i < prfxLen; i++) {
    CERR << (sLine[i] == '\t' ? '\t' : ' ');
  }
  // now underline the error
  CERR << sUline << ENDL;
}

//------------------------------------------------------------------------------
// dumpEEPROMLayout
//------------------------------------------------------------------------------
void Compiler::dumpEEPROMLayout()
{
  static bool once(true);

  CERR << "EEPROM layout:" << std::endl;
  CERR << "  0 empty, . undefined, # defined, P program" << std::endl << std::endl;

  int nUsed(0);
  const int linesz = 16;
  const int lines  = EEsz / linesz;

  byte* pEEFlags = m_pRec->EEPROMFlags;

  CERR << 
    "           0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f" << std::endl;

  for (int k = 0; k < lines; k++) {
    bool bNull = true;
    for (int i = 0; i < linesz; i++) {
      if (pEEFlags[i] & used) {
        bNull = false;
        break;
      }
    }

    if (bNull) {
      if (once) {
        CERR << "-unused-" << std::endl;
        once = false;
      }
    }
    else {
      once = true;

      int lineadr = k * linesz;
      CERR << std::setfill('0') <<
        std::setw(4) << std::hex << lineadr << "(" <<
        std::setw(4) << std::dec << lineadr << ") "; 

      for (int i = 0; i < linesz; i++) {
        if (pEEFlags[i] & used) {
          if ((pEEFlags[i] & usage) == empty)
            CERR << "0  ";

          if ((pEEFlags[i] & usage) == undefined) 
            CERR << ".  ";

          if ((pEEFlags[i] & usage) == defined) {
#ifdef _COLORFUL
            CERR << CEEDEFINED << "#  " << CNONE;
#else
            CERR << "#  ";
#endif
            nUsed++;
          }

          if ((pEEFlags[i] & usage) == program) {
#ifdef _COLORFUL
            CERR << CEEPROGRAM << "P  " << CNONE;
#else
            CERR << "P  ";
#endif
            nUsed++;
          }
        }

        else {
          CERR << "   ";
        }
      }

      CERR << std::endl;
    }

    pEEFlags += linesz;
  }

  CERR << std::endl;
}

//------------------------------------------------------------------------------
// showPcntUsage
//------------------------------------------------------------------------------
void Compiler::showPcntUsage()
{
  int nUsed = 0;

  byte* pEEFlags = m_pRec->EEPROMFlags;

  for (int i = 0; i < EEsz; i++) {
    byte u = pEEFlags[i] & usage;
    if (u == defined || u == program) nUsed++;
  }

  // setup a percentage bar plus numerical usage display
  int pcntUsg = (100 * nUsed) / EEsz;
  int pcntBar = ((2 * pcntUsg) + 5) / 10;
  std::string sBar1, sBar2;
  sBar1.assign(pcntBar, '#');
  sBar2.assign(20 - pcntBar, '-');

  CERR << "defined data & program memory usage: " << 
    '[' << sBar1 << sBar2 << ']' <<
    pcntUsg << "%" << std::endl << std::endl; 
}

//------------------------------------------------------------------------------
// dumpEEPROM
//------------------------------------------------------------------------------
void Compiler::dumpEEPROM()
{
  static bool once(true);

  CERR << "EEPROM contents:" << std::endl;
  CERR << "  defined data & program" << std::endl << std::endl;

  const int linesz = 16;
  const int lines  = EEsz / linesz;

  byte* pEEData  = m_pRec->EEPROM;
  byte* pEEFlags = m_pRec->EEPROMFlags;

  CERR << 
    "           0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f" << 
    "   0123456789abcdef" << std::endl;

  for (int k = 0; k < lines; k++) {
    bool bNull = true;
    for (int i = 0; i < linesz; i++) {
      if (pEEFlags[i] & used) {
        bNull = false;
        break;
      }
    }

    if (bNull) {
      if (once) {
        CERR << "-unused-" << std::endl;
        once = false;
      }
    }
    else {
      once = true;

      int lineadr = k * linesz;
      CERR << std::setfill('0') << 
        std::setw(4) << std::hex << lineadr << "(" <<
        std::setw(4) << std::dec << lineadr << ") "; 

      // hex display
      for (int i = 0; i < linesz; i++) {
#ifdef _COLORFUL
        if ((pEEFlags[i] & usage) == program) CERR << CEEPROGRAM;
        else if ((pEEFlags[i] & usage) == defined) CERR << CEEDEFINED;
#endif

        CERR <<
          std::setw(2) << std::hex << (int)pEEData[i] << " ";

#ifdef _COLORFUL
        CERR << CNONE;
#endif
      }

      CERR << " ";

      // printable data display
      for (int i = 0; i < linesz; i++) {
        if (pEEFlags[i] & used) {
          if ((pEEFlags[i] & usage) == defined) {
            char c = pEEData[i];
            CERR << (isprint(c) ? c : '.');
          }
        }

        else {
          CERR << " ";
        }
      }

      CERR << std::endl;
    }

    pEEData += linesz;
    pEEFlags += linesz;
  }

  CERR << std::endl;
}

//------------------------------------------------------------------------------
// dumpVariables
//------------------------------------------------------------------------------
void Compiler::dumpVariables()
{
  CERR << "RAM register usage:" << std::endl;
  CERR << "  W word, B byte, N nibble, L bit" << std::endl << std::endl;

  int i;
  int j(0);

  byte* pVar = m_pRec->VarCounts;

  CERR << 
    "      15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0";
//  "INS                     -- WORD --" << std::endl <<
//  "OUTS                    -- WORD --" << std::endl <<
//  "DIRS                    -- WORD --" << std::endl <<

  for (i = 0; i < pVar[3]; i++) {
    CERR << std::endl << NEXT_REG(j);
#ifdef _COLORFUL
    CERR << CEEWORD << "W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W. " <<
      CNONE;
#else
    CERR << "W  W  W  W  W  W  W  W  W  W  W  W  W  W  W  W. ";
#endif
    j += 16;
  }

  for (i = 0; i < pVar[2]; i++) {
    if (j % 16 == 0) CERR << std::endl << NEXT_REG(j);
#ifdef _COLORFUL
    CERR << CEEBYTE << "B  B  B  B  B  B  B  B. " << CNONE;
#else
    CERR << "B  B  B  B  B  B  B  B. ";
#endif
    j += 8;
  }

  for (i = 0; i < pVar[1]; i++) {
    if (j % 16 == 0) CERR << std::endl << NEXT_REG(j);
#ifdef _COLORFUL
    CERR << CEENIBBLE << "N  N  N  N. " << CNONE;
#else
    CERR << "N  N  N  N. ";
#endif
    j += 4;
  }

  for (i = 0; i < pVar[0]; i++) {
    if (j % 16 == 0) CERR << std::endl << NEXT_REG(j);
#ifdef _COLORFUL
    CERR << CEEBIT << "L. " << CNONE;
#else
    CERR << "L. ";
#endif
    j++;
  }

  CERR << std::endl << std::endl;
}

//------------------------------------------------------------------------------
// dumpEtc
//------------------------------------------------------------------------------
void Compiler::dumpEtc()
{
  // print language version
  int lvmajor = m_pRec->LanguageVersion / 100;
  int lvminor = m_pRec->LanguageVersion % 100;

  CERR << "language version " <<
    lvmajor << "." <<
    lvminor / 10;
  if (lvminor % 10) CERR << lvminor % 10;
  CERR << std::endl;
}

