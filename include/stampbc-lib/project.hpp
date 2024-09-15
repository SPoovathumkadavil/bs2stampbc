#pragma once

#include <string>

#include "compiler.hpp"
#include "loader.hpp"

class Project 
{
public:
  // Compiler Flags
  enum { F_VERBOSE = 1, F_COMPILEONLY = 2, F_DOWNLOADONLY = 4,
    F_IDENTIFYONLY = 8, F_VERSIONINFO = 16, F_IGNPROJECT = 32,
    F_SYNTAXONLY = 64, F_OVRPORT = 128, F_OVRSTAMP = 256,
    F_MEMORYMAP = 512, F_MEMORYUSAGE = 1024 };

  Project();
  virtual ~Project();

  void setFlags(int n);
  void setSlot(int n);
  void setPort(const std::string& s);
  void setType(const std::string& s);
  void setSourceFile(int i, const std::string& s);
  void setObjectFile(int i, const std::string& s);

  bool   isFlags(int n);
  bool   isFlagsAnd(int n);
  int    getFlags();
  int    getSlot();
  std::string getPort();
  std::string getType();
  std::string getSourceFile(int i);
  std::string getObjectFile(int i);

  std::string makeObjectFileName(const std::string& src);

  bool process();

private:
  void versionInfo();
  bool identify();
  bool compile();
  bool download();

  void setupCompiler(Compiler* pCmp);
  void setupLoader(Loader* pLdr);

  std::string m_sSourceFile[8];
  std::string m_sObjectFile[8];

  std::string m_sType;
  std::string m_sPort;

  int m_nFlags;
  int m_iSlot;
};
