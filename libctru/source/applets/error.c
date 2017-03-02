#include <3ds.h>
#include <stdlib.h>
#include <string.h>
#include <3ds/types.h>
#include <3ds/svc.h>
#include <3ds/synchronization.h>
#include <3ds/services/apt.h>
#include <3ds/services/cfgu.h>
#include <3ds/util/utf.h>
#include <3ds/applets/error.h>

void error_Init(errorConf* err, errorType type, CFG_Language lang)
{
	memset(err, 0, sizeof(*err));
	err->errorType=type;
	err->useLanguage=lang;
	err->upperScreenFlag= ERROR_NORMAL;
	err->eulaVersion =  0;
	err->homeButton  = true;
	err->softwareReset = false;
	err->appJump     = false;
	err-> returnCode   = ERROR_UNKNOWN;
}

static void errorConvertToUTF16(u16* out, const char* in, size_t max)
{
	if (!in || !*in)
	{
		out[0] = 0;
		return;
	}

	ssize_t units = utf8_to_utf16(out, (const uint8_t*)in, max-1);
	if (units < 0)
	{
		out[0] = 0;
		return;
	}

	out[units] = 0;
}

static char* c_shift(const char *text)
{
	size_t len = strlen(text);
	char *str = (char*)malloc(len + 2);
	if (str)
	{
		str[0] = text[0];
		strcpy(&str[1], text);
	}
	return str;
}

void error_code(errorConf* err, int error)
{
	err->errorCode = error;
}

void error_text(errorConf *err, const char* text)
{	char *tex = c_shift(text);
	errorConvertToUTF16(err->Text, tex, 1900);
	free(tex);
}

void error_disp(errorConf* err)
{   
	aptLaunchLibraryApplet(APPID_ERROR, err, sizeof(*err), 0);
}
