
#include <cstdio>
#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>

#include "project.hpp"

//------------------------------------------------------------------------------
// Project
//------------------------------------------------------------------------------
Project::Project():
  m_sType(STAMPID_BS2),
  m_sPort(COMM_DEVICE_DEFAULT),
  m_nFlags(0),
  m_iSlot(-1)
{}

//------------------------------------------------------------------------------
// ~Compiler
//------------------------------------------------------------------------------
Project::~Project()
{
  Loader::free();
  Compiler::free();
}

//------------------------------------------------------------------------------
// set
//------------------------------------------------------------------------------
void Project::setFlags(int n) { m_nFlags = n; }
void Project::setSlot(int n) { m_iSlot = n; }
void Project::setPort(const std::string& s) { m_sPort = s; }
void Project::setType(const std::string& s) { m_sType = s; }
void Project::setSourceFile(int i, const std::string& s) { m_sSourceFile[i] = s; }
void Project::setObjectFile(int i, const std::string& s) { m_sObjectFile[i] = s; }

//------------------------------------------------------------------------------
// get
//------------------------------------------------------------------------------
bool   Project::isFlags(int n) { return (m_nFlags & n) != 0; }
bool   Project::isFlagsAnd(int n) { return (m_nFlags & n) == n; }
int    Project::getFlags() { return m_nFlags; }
int    Project::getSlot() { return m_iSlot; }
std::string Project::getPort() { return m_sPort; }
std::string Project::getType() { return m_sType; }
std::string Project::getSourceFile(int i) { return m_sSourceFile[i]; }
std::string Project::getObjectFile(int i) { return m_sObjectFile[i]; }

//------------------------------------------------------------------------------
// makeObjectFileName
//------------------------------------------------------------------------------
std::string Project::makeObjectFileName(const std::string& src)
{
  int ndot = src.rfind('.');
  if (ndot != -1) {
    return src.substr(0, ndot).append(".o");
  }
  else {
    return src + ".o";
  }
}

//------------------------------------------------------------------------------
// process
//------------------------------------------------------------------------------
bool Project::process()
{
  if (isFlags(F_VERSIONINFO)) { 
    versionInfo();
    return true;
  }

  if (isFlags(F_IDENTIFYONLY)) {
    return identify();
  }

  bool comp(true);

  if (!isFlags(F_DOWNLOADONLY)) {
    comp = compile();
    if (isFlags(F_COMPILEONLY | F_SYNTAXONLY)) return comp;
  }

  if (comp) return download();

  return false;
}

//------------------------------------------------------------------------------
// identify
//------------------------------------------------------------------------------
bool Project::identify()
{
  Loader* ldr   = Loader::instance();

  setupLoader(ldr);

  std::string sType;
  std::string sVersion;
  int index;

  return ldr->identify(sType, sVersion, index);
}

//------------------------------------------------------------------------------
// compile
//------------------------------------------------------------------------------
bool Project::compile()
{
  Compiler* cmp = Compiler::instance();

  setupCompiler(cmp);

  bool directivesOnly(false);
  bool parseStampDirective(true);
  std::string sType("");
  std::string sPort("");

  // override stamp directive - use specified type
  if (isFlags(F_OVRSTAMP)) {
    parseStampDirective = false;
    sType = m_sType;
  }

  // compile first source
  bool compiled = cmp->compile(m_sSourceFile[0], m_sObjectFile[0],
    sType, sPort, directivesOnly, parseStampDirective, true);

  if (compiled && isFlags(F_MEMORYMAP | F_MEMORYUSAGE))
    cmp->dump(isFlags(F_MEMORYMAP));

  // use stamp type specified by stamp directive from the source
  if (!isFlags(F_OVRSTAMP)) {
    m_sType = sType;
  }

  // use the port specified by stamp directive from the source
  if (!isFlags(F_OVRPORT) && sPort != "") {
    m_sPort = sPort;
  }

  // compile other sources of this project
  if (compiled && !isFlags(F_IGNPROJECT)) {
    std::string src;

    int i(1);

    while ((src = cmp->getNextSource()) != "") {
       std::string obj = makeObjectFileName(src);

       m_sSourceFile[i] = src;
       m_sObjectFile[i] = obj;

       compiled = cmp->compile(src, obj, sType, sPort,
         directivesOnly, parseStampDirective, false);

       if (compiled) {
         if (isFlags(F_MEMORYMAP | F_MEMORYUSAGE))
           cmp->dump(isFlags(F_MEMORYMAP));
       }
       else {
         break;
       }

       i++;
    }
  }

  return compiled;
}

//------------------------------------------------------------------------------
// download
//------------------------------------------------------------------------------
bool Project::download()
{
  Loader* ldr = Loader::instance();
  bool loaded(true);

  setupLoader(ldr);

  bool single = m_sObjectFile[1] == "";

  if (isFlags(F_DOWNLOADONLY) || single) {
    loaded = ldr->download(m_sObjectFile[0], m_sType, m_iSlot);
  }
  else {
    for (int i = 0; i < 8 && loaded; i++) {
      std::string obj = m_sObjectFile[i];
      if (obj == "") break;

      loaded = ldr->download(m_sObjectFile[i], m_sType, i);
    }
  }

  return loaded;
}

//------------------------------------------------------------------------------
// versionInfo
//------------------------------------------------------------------------------
void Project::versionInfo()
{
  Compiler* cmp = Compiler::instance();

  setupCompiler(cmp);

  CERR << STAMPBC_VERSION << ", ";
  cmp->printVersionInfo();
}

//------------------------------------------------------------------------------
// setupCompiler
//------------------------------------------------------------------------------
void Project::setupCompiler(Compiler* pCmp)
{
  if (isFlags(F_SYNTAXONLY)) {
    m_sObjectFile[0] = "";
  }

  else if (m_sObjectFile[0] == "") {
    m_sObjectFile[0] = makeObjectFileName(m_sSourceFile[0]);
  }

  pCmp->setDebug(isFlags(F_VERBOSE));
}

//------------------------------------------------------------------------------
// setupLoader
//------------------------------------------------------------------------------
void Project::setupLoader(Loader* pLdr)
{
  pLdr->setPort(m_sPort);
  pLdr->setDebug(isFlags(F_VERBOSE));
}