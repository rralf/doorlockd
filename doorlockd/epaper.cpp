#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include "logger.h"

extern "C" {
#include "epaper/bsp.h"
#include "epaper/Display_Controller.h"
}

#include "epaper.h"
#include "config.h"

using namespace std;

Epaper::Epaper() :
    _logger(Logger::get())
{
    memset(_prevImg, 0xFF, _ARRAY_SIZE);
    // Initialize Epaper library
    bsp_init();
}

Epaper::~Epaper()
{
}

Epaper &Epaper::get()
{
    static Epaper e;
    return e;
}

void Epaper::draw(const string &uri)
{
    unsigned char buffer[_ARRAY_SIZE];
    snprintf((char*)buffer, _ARRAY_SIZE,
             "qrencode -l M -d 100 -s 5 \"%s\" -t png -o -"
             "| composite -geometry +90+0 /dev/stdin " TEMPLATE_LOCATION " -colorspace gray -depth 1 gray:-",
             uri.c_str());

    FILE* f = popen((const char*)buffer, "r");
    int i = fread(buffer, _ARRAY_SIZE, 1, f);
    if (i != 1)
    {
        _logger(LogLevel::error, "Image format error");
        pclose(f);
        return;
    }
    pclose(f);

    epd_DisplayImg(EPDType_270, buffer, _prevImg);
    memcpy(_prevImg, buffer, _ARRAY_SIZE);
}
