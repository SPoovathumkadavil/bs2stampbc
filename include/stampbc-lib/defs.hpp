#pragma once

//******************************************************************************
// defs.hpp
//
// ASC20021014 created
// ASC20021113 added STAMPID_, COMM_
// ASC20030417
// ASC20040203 version 2.0
// ASC20040203 version 2.1
//******************************************************************************

#undef byte
typedef unsigned char byte;

#undef bool
#define bool byte

#undef word
typedef unsigned short int word;

#define STAMPBC_VERSION		"stampbc v0.1.0(2024)"

#define COMM_DEVICE		"/dev/tty.usbserial"                // macos
#define COMM_DEVICE_DEFAULT	"/dev/tty.usbserial-A8004JO5" // macos

#define RECV_TIMEOUT_S          0
#define RECV_TIMEOUT_US         250000

#define RESET_DTRPULSE_US	5000
#define BREAK_TXPULSE_US	36000
#define BREAK_TXRELAX_US	20000

#define PACKET_BUFFER_SZ	2304

// stamp IDs - order as required by tokenizer.so
#define STAMPID_NONE		"none"
#define STAMPID_RESERVED	"reserved"
#define STAMPID_BS2		"BS2"
#define STAMPID_BS2E		"BS2e"
#define STAMPID_BS2SX		"BS2sx"
#define STAMPID_BS2P		"BS2p"
#define STAMPID_BS2PE		"BS2pe"
#define STAMPID_BS2PX		"BS2px"

// preliminary stamp support: compile for a different type instead
#define FOREIGN_STAMP		1
#define STAMPID_FOREIGN		STAMPID_BS2PX
#define STAMPID_FOREIGNTARGET	STAMPID_BS2PE

#define CERR			std::cout

// eye catchers
#ifdef _COLORFUL
#define EYEERR			"\033[31m**"
#define EYEERC			"\033[31m  "
#define EYEINF			"\033[34m--"
#define EYEINC			"\033[34m  "
#define ENDL			"\033[0m\n"
#else
#define EYEERR			"**"
#define EYEERC			"  "
#define EYEINF			"--"
#define EYEINC			"  "
#define ENDL			std::endl
#endif

// color sequences
#define CBLKGRN			"\033[30;42m"
#define CBLKYLW			"\033[30;43m"
#define CBLKCYN			"\033[30;46m"
#define CBLKGRY			"\033[30;47m"
#define CWHTBLK			"\033[37;40m"
#define CWHTRED			"\033[37;41m"
#define CWHTBLU			"\033[37;44m"
#define CWHTMGT			"\033[37;45m"
#define CNONE			"\033[0m"

// abstract colors
#define CEEPROGRAM		CBLKYLW
#define CEEDEFINED		CBLKCYN
#define CEEWORD			CBLKYLW
#define CEEBYTE			CBLKGRN
#define CEENIBBLE		CBLKCYN
#define CEEBIT			CBLKGRY
