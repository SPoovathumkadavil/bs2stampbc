#pragma once

//******************************************************************************
// loader.hpp
//
// ASC20021021 created
// ACS20021112 stamp version string introduced
//******************************************************************************

#include <string>

#include "defs.hpp"

struct StampID {
  char* send;
  char* recv;
  char* id;
  int   memSlots;
};

class Loader
{
public:
  static const int packetSize;

  static Loader* instance();
  static void free();

  void setPort(std::string sPort);
  void setDebug(bool dbg);

  int typeToIndex(const std::string& sType);
  std::string versionToVersionID(const std::string& sVer);

  bool identify(std::string& sType, std::string& sVersion, int& index);
  bool download(const std::string objFile, std::string& sType, int slot);

private:
  Loader();
  virtual ~Loader();

  long fileSize(const std::string fileName);
  void resetDev(bool brkcond = false);
  void closeDev();
  void waitRecv(bool bQuiet);
  void receive(const char* pBuffer, int n, int& nReti, bool bQuiet = false);
  void send(const char* pBuffer, int n);
  void sendBreakCondition();
  void sendIdChars(const char* toSend, const char* toRecv, std::string& sVer,
    bool isBS2);
  void downloadPacket(const char* pBuffer, int packnum);

  int  m_fp;
  bool m_done;
  int  m_sel;
  bool m_dbg;

  std::string m_port;

  static Loader* m_pInstance;
  static const StampID m_stampID[];
  static const char* m_versionID[][2];
};
