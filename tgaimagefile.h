#ifndef TGAIMAGEFILE_H_
#define TGAIMAGEFILE_H_

#include <Windows.h>

#define WIDTH 80
#define HEIGHT 60

// NOTE(not-set): 24 bit color
// 0x ff ff ff
//    r  g  b
// each color is 8 bits

// The header is 18
// byte alignment can take place so
// be careful using fread
struct TGAHeader {
    char IdLength;
    char ColorMapType;
    char DataTypeCode;
    short int ColorMapOrigin;
    short int ColorMapLength;
    char ColorMapDepth;
    short int XOrigin;
    short int YOrigin;
    short width;
    short height;
    char BitsPerPixel;
    char ImageDescriptor;
};

struct Image {
    char *buffer;
    int width;
    int height;
    int bpp;
    int imageHeadSize;
    int bufSize;
};

union RGBAColors {
    struct {
        char b;
        char g;
        char r;
        char a;
    };
    unsigned int color;
    char colors[4];
};

typedef struct Image Image;
typedef struct TGAHeader TGAHeader;
typedef union RGBAColors RGBAColors;

#define WRITE_HEADER_FIELD(field, buffer, offset) \
do { \
int fieldSize = sizeof(field); \
if (fieldSize > 1) { \
*(buffer + offset) = (field & 0xFF); \
offset++; \
*(buffer + offset) = ((field >> 8) & 0xFF); \
offset++; \
} else { \
*(buffer + offset) = field; \
offset++; \
} \
} while(0)

void InitializeTGAFile(Image *image) {
    // create a header size is 18
    TGAHeader header;
    header.IdLength = 0;
    header.ColorMapType = 0;
    header.DataTypeCode = 2;
    // Map is ignored when ColorMapType is 0
    header.ColorMapOrigin = 0;
    header.ColorMapLength = 0;
    header.ColorMapDepth = 0;
    header.XOrigin = 0;
    header.YOrigin = 0;
    header.width = image->width;
    header.height = image->height;
    header.BitsPerPixel = image->bpp;
    header.ImageDescriptor = 8;
    
    
    image->bufSize = ((image->width * image->height) * ((image->bpp) / 8));
    image->bufSize += 18; // using 18 to avoid error by optimization
    
    image->imageHeadSize = 18 + header.IdLength;
    
    image->buffer = (char *)VirtualAlloc(0, image->bufSize,
                                         MEM_COMMIT|MEM_RESERVE,
                                         PAGE_READWRITE);
    if (!image->buffer) {
        return ;
    }
    
    int offset = 0;
    
    WRITE_HEADER_FIELD(header.IdLength, image->buffer, offset);
    WRITE_HEADER_FIELD(header.ColorMapType, image->buffer, offset);
    WRITE_HEADER_FIELD(header.DataTypeCode, image->buffer, offset);
    WRITE_HEADER_FIELD(header.ColorMapOrigin, image->buffer, offset);
    WRITE_HEADER_FIELD(header.ColorMapLength, image->buffer, offset);
    WRITE_HEADER_FIELD(header.ColorMapDepth, image->buffer, offset);
    WRITE_HEADER_FIELD(header.XOrigin, image->buffer, offset);
    WRITE_HEADER_FIELD(header.YOrigin, image->buffer, offset);
    WRITE_HEADER_FIELD(header.width, image->buffer, offset);
    WRITE_HEADER_FIELD(header.height, image->buffer, offset);
    WRITE_HEADER_FIELD(header.BitsPerPixel, image->buffer, offset);
    WRITE_HEADER_FIELD(header.ImageDescriptor, image->buffer, offset);
}

void TGADrawPixel(int x, int y, RGBAColors color, Image *image) {
    int offset = image->imageHeadSize;
    int width = image->width;
    int bypp = image->bpp / 8; // byteperpixel
    
    image->buffer[(((y * width) + x) * bypp) + offset] = color.b;
    offset++;
    
    image->buffer[(((y * width) + x) * bypp) + offset] = color.g;
    offset++;
    
    image->buffer[(((y * width) + x) * bypp) + offset] = color.r;
    offset++;
    
    image->buffer[(((y * width) + x) * bypp) + offset] = color.a;
}

void TGAFill(RGBAColors color, Image *image) {
    int offset = image->imageHeadSize;
    
    for (int Y = 0; Y < image->height; Y++) {
        for (int X = 0; X < image->width; X++) {
            
            image->buffer[(Y * image->width) + X + offset] = color.b;
            offset++;
            
            image->buffer[(Y * image->width) + X + offset] = color.g;
            offset++;
            
            image->buffer[(Y * image->width) + X + offset] = color.r;
            offset++;
            
            image->buffer[(Y * image->width) + X + offset] = color.a;
            
        }
    }
}

void TGAVerticalFlip() {
}

#endif