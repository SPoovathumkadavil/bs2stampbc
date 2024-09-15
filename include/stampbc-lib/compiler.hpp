//******************************************************************************
// SBCompiler.h
//
// ASC20021014 created
// ASC20021021 adapted for tokenizer.so v1.16
// ASC20021119 dump methods added
//******************************************************************************

#pragma once

#include <tokenizer/tokenizer.hpp>
#include <string>

#include "defs.hpp"

#define PACKETSIZE 18

class Compiler 
{
public:
  static Compiler* instance();
  static void free();

  bool testRecAlignment();
  void printVersionInfo();
  bool compile(
    const std::string& srcFile,
    const std::string& objFile, 
    std::string& sType,
    std::string& sPort,
    bool directivesOnly = false, 
    bool parseStampDirective = true,
    bool isProject = true
  );
  void dump(bool bMap);
  std::string getNextSource();
  void setDebug(bool b);
  std::string moduleIDToStampID(int mid);
  int stampIDToModuleID(std::string sid);

private:
  // size of the EEPROM array
  enum {EEsz = 2048};

  // EEPROM bitsets & flags
  enum {used = 0x80, usage = 0x7f};
  enum {empty = 0, undefined = 1, defined = 2, program = 3};
  
  Compiler();
  virtual ~Compiler();

  int readSourceFile(const std::string& srcFile, char* srcBuffer);
  int writeObjectFile(const std::string& objFile, byte* pckBuffer, int pckCount);
  void printOffendingLine(TModuleRec *rec, char* src);
  void dumpEEPROMLayout();
  void showPcntUsage();
  void dumpEEPROM();
  void dumpVariables();
  void dumpEtc();

  static Compiler* m_pInstance;
  static const char* m_stampID[];

  tokenizer*   m_pTk;
  int          m_index;
  std::string  m_sSource[7];
  bool         m_dbg;
  TModuleRec*  m_pRec;
};
