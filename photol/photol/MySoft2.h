#pragma once
#include <string>

using namespace std;


typedef struct _lic_header
{
	unsigned char version;
	unsigned char mode;
	unsigned short num;
	unsigned short chksum;
	unsigned char ov;
	unsigned char year;
	unsigned long long date;
} lic_header;

#define MODE_SIGMA 10
#define MODE_STAR 20
#define MODE_EXE 30

#define ERROR_SIGN 4
#define ERROR_LIC 5
#define ERROR_OVERDUE 6
#define BF_PASSWORD "Thamos9d"

#define EXP_SPAN 24 * 60 * 60

class CMySoft
{
public:
	CMySoft(void);
	~CMySoft(void);

public:
	int Verify(string s2, lic_header &lic);
	int get_verify(string &code, lic_header &lic, int &ov);
	long add_lic(string key, string val);
	void get_lic(string & v0, string & v3);
	int Reg(string s2, lic_header &lic);
	BOOL Grn(lic_header lic, string& sLic);
	int ReadLic(lic_header &lic);
	int WriteLic(lic_header lic);

private:
	void get_first(string &first);

private:
	void ScrubBlob(void *b, DWORD cb);
};
