#include <efi.h>
#include <efilib.h>
#include <efiprot.h>

#include <kernel.h>
#include "boot.h"

struct GGraphics GGraphics;

EFI_STATUS initGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol);
int32_t* getInitAddress(int w, int h);
EFI_STATUS getMode(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol, OUT UINT32 *Mode);
uint32_t rgb2bgr(uint32_t rgb);
uint32_t bgr2rgb(uint32_t bgr);
int lineBit;

// Well, not really graphics driver but.

EFI_STATUS initGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol) {
  EFI_STATUS status;
  UINT32 newMode;
  Print(L"Selecting Mode\n");
  status = getMode(GProtocol, &newMode);
  status = uefi_call_wrapper(GProtocol->SetMode, 2, GProtocol, newMode);

  //Initialize GGraphics
  Print(L"Initializing Graphics\n");
  GGraphics.GProtocol = GProtocol;
  GGraphics.PixelFormat = GGraphics.info.PixelFormat;
  GGraphics.FrameBufferBase = GProtocol->Mode->FrameBufferBase;
  GGraphics.FrameBufferSize = GProtocol->Mode->FrameBufferSize;
  GGraphics.lineBits = 4 * GGraphics.info.PixelsPerScanLine;

  return status;
}

EFI_STATUS getMode(EFI_GRAPHICS_OUTPUT_PROTOCOL *GProtocol, OUT UINT32 *Mode) {
  UINTN size;
  UINT32 maxMode = GProtocol->Mode->MaxMode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *modeInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *selectedModeInfo;
  *Mode = GProtocol->Mode->Mode;

  uefi_call_wrapper(GProtocol->QueryMode, 4, GProtocol, *Mode, &size, &selectedModeInfo);

  // List all the mode avaliable:
  for (UINT32 i = 0; i < maxMode; i += 1) {
    // Fetching each avaliable graphic mode.
    uefi_call_wrapper(GProtocol->QueryMode, 4, GProtocol, i, &size, &modeInfo);

    //Only accept RGB8Bit or BGR8Bit Pixel format. Other format skipped.
    if (modeInfo->PixelFormat != PixelRedGreenBlueReserved8BitPerColor &&
        modeInfo->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
      continue;
    }
    //Select desired resolution. Filter overly big resolution. Aim for bigger resolution.
    if (modeInfo->HorizontalResolution > DESIRED_HORIZONTAL_RESOLUTION ||
        modeInfo->VerticalResolution > DESIRED_VERTICAL_RESOLUTION) {
      continue;
    }
    if (modeInfo->HorizontalResolution >= selectedModeInfo->HorizontalResolution &&
        modeInfo->VerticalResolution >= selectedModeInfo->VerticalResolution) {
      selectedModeInfo = modeInfo;
      *Mode = i;
    }
  }

  //Initialize GGrapics Should move into initGraphics
  GGraphics.info = *selectedModeInfo;
  GGraphics.PixelFormat = selectedModeInfo->PixelFormat;

  //Output Information
  Print(L"Selected Mode: #%d Width: %d, Height: %d. Pixel Format: ", *Mode,
        selectedModeInfo->HorizontalResolution,
        selectedModeInfo->VerticalResolution);
  if (selectedModeInfo->PixelFormat == PixelRedGreenBlueReserved8BitPerColor)
    Print(L"PixelRedGreenBlueReserved8BitPerColor\n");
  if (selectedModeInfo->PixelFormat == PixelBlueGreenRedReserved8BitPerColor)
    Print(L"PixelBlueGreenRedReserved8BitPerColor\n");

  return EFI_SUCCESS;
}

int32_t* getInitAddress(int w, int h) {
  return (int32_t *)(GGraphics.FrameBufferBase + w * 4 + h * GGraphics.lineBits);
}

//Literally fill the given position with the given pixel. Should only be called within the file
//Surprisingly Slow
void setPixel(int w, int h, uint32_t value) {
  //Multiply by 4 since 1 unit of memory address contains 8 btis. Meaning that each pixel gives 32bit offset.
  int32_t *addr = getInitAddress(w, h);
  *addr = 0xff000000 | value;
}

//Could be optimized through storing the bool operation in a var.

uint32_t fromRGB(uint32_t rgb) {
  if (GGraphics.PixelFormat == PixelRedGreenBlueReserved8BitPerColor)
    return rgb;
  else
    return ((0xff0000 & rgb) >> 16) | (0xff00 & rgb) | ((0xff & rgb) << 16);
}

uint32_t fromBGR(uint32_t bgr) {
  if (GGraphics.PixelFormat == PixelBlueGreenRedReserved8BitPerColor)
    return bgr;
  else
    return ((0xff0000 & bgr) >> 16) | (0xff00 & bgr) | ((0xff & bgr) << 16);
}

//

void setRGBPixel(int w, int h, uint32_t rgb) {
  setPixel(w, h, fromRGB(rgb));
}

void setBGRPixel(int w, int h, uint32_t bgr) {
  setPixel(w, h, fromBGR(bgr));
}

void clearScreen(uint32_t rgb) {
  int32_t *addr = (int32_t *)GGraphics.FrameBufferBase;
  for (int i = 0; i < GGraphics.GProtocol->Mode->FrameBufferSize / 4; i += 4) {
    *addr++ = 0xff000000 | rgb;
    *addr++ = 0xff000000 | rgb;
    *addr++ = 0xff000000 | rgb;
    *addr++ = 0xff000000 | rgb;
  }
}

void drawRectangle(uint32_t rgb, int width, int height, int x, int y) {
  int32_t *addr = getInitAddress(x, y);
  for (int i = 0; i < height; i += 1) {
      for (int k = 0; k < width; k++) {
        // I think this is where the inefficiency happens?
        *addr++ = 0xff000000 | rgb;
      }
      addr += GGraphics.lineBits / 4 - width;
  }

}
