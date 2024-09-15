
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

#include "project.hpp"

using namespace std;

void printUsage()
{
  CERR << "stampbc [OPTIONS] [FILENAME]" << endl;
  CERR << "  -c       compile single file only" << endl;
  CERR << "  -C       compile project only" << endl;
  CERR << "  -d       download only" << endl;
  CERR << "  -h       help" << endl;
  CERR << "  -i type  identify specified stamp type" << endl;
  CERR << "  -j       let program identify the stamp" << endl;
  CERR << "  -m       output memory usage" << endl;
  CERR << "  -M       output memory map and usage" << endl;
  CERR << "  -n       syntax check only" << endl;
  CERR << "  -o file  with -c use object file name" << endl;
  CERR << "  -p port  use specified port for download" << endl;
  CERR << "  -s slot  download to specified slot" << endl;
  CERR << "  -v       verbose" << endl;
  CERR << "  -V       print stampbc and tokenizer version" << endl;
}

//------------------------------------------------------------------------------
// processOptions
//
// -c       compile single file only
// -C       compile project only
// -d       download only
// -h       help
// -i type  identify specified stamp type
// -j       let program identify the stamp
// -m       output memory usage
// -M       output memory map and usage
// -n       syntax check only
// -o file  with -c use object file name
// -p port  use specified port for download
// -s slot  download to specified slot
// -v       verbose
// -V       print stampbc and tokenizer version
//------------------------------------------------------------------------------
int processOptions(int argc, char** argv, Project& proj)
{
  static char options[] = "cCdhi:jmMno:p:s:vV";
  int opt;
  int f;
  string objFile;

  if (argc < 2) {
    printUsage();
    return 1;
  }

  while ((opt = getopt(argc, argv, options)) != -1) {
    switch (opt) {
      case 'c':
        f = proj.getFlags() | Project::F_COMPILEONLY
            | Project::F_IGNPROJECT;
        proj.setFlags(f);
        break;

      case 'C':
        f = proj.getFlags()
            | Project::F_COMPILEONLY & ~Project::F_IGNPROJECT;
        proj.setFlags(f);
        break;

      case 'd':
        f = proj.getFlags() | Project::F_DOWNLOADONLY;
        proj.setFlags(f);
        break;

      case 'h':
        printUsage();
        return 1;
        break;

      case 'i':
        f = proj.getFlags() | Project::F_OVRSTAMP;
        proj.setFlags(f);
        proj.setType(string(optarg));
        break;

      case 'j':
        f = proj.getFlags() | Project::F_IDENTIFYONLY;
        proj.setFlags(f);
        break;

      case 'm':
        f = proj.getFlags() | Project::F_MEMORYUSAGE;
        proj.setFlags(f);
        break;

      case 'M':
        f = proj.getFlags() | Project::F_MEMORYMAP;
        proj.setFlags(f);
        break;

      case 'n':
        f = proj.getFlags() | Project::F_SYNTAXONLY | Project::F_IGNPROJECT;
        proj.setFlags(f);
        break;

      case 'o':
        objFile = string(optarg);
        break;

      case 'p':
        f = proj.getFlags() | Project::F_OVRPORT;
        proj.setFlags(f);
        proj.setPort(string(optarg));
        break;

      case 's':
        proj.setSlot(atoi(optarg));
        break;

      case 'v':
        f = proj.getFlags() | Project::F_VERBOSE;
        proj.setFlags(f);
        break;

      case 'V':
        f = proj.getFlags() | Project::F_VERSIONINFO;
        proj.setFlags(f);
        break;

      default:
        return -1;
    }
  }

  if (optind < argc) {
    string fileName(argv[optind]);

    if (proj.isFlagsAnd(Project::F_COMPILEONLY | Project::F_IGNPROJECT)) {
      proj.setObjectFile(0, objFile);
    }

    if (proj.isFlags(Project::F_DOWNLOADONLY)) {
      proj.setObjectFile(0, fileName);
    } else {
      proj.setSourceFile(0, fileName);
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Project prj;

  int ret = processOptions(argc, argv, prj);

  if (ret == 0) {
    ret = prj.process() ? 0 : -1;
  }

  // ret may be > 0 from processOptions, that is ok
  exit(ret > 0 ? 0 : ret);
};
