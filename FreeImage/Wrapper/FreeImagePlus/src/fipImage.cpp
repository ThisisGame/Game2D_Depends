// ==========================================================
// fipImage class implementation
//
// Design and implementation by
// - Herv� Drolon (drolon@infonie.fr)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#include "FreeImagePlus.h"

///////////////////////////////////////////////////////////////////   
// Protected functions

FI_BOOL fipImage::replace(FIBITMAP *new_dib) {
	if(new_dib == NULL) 
		return FALSE;
	if(_dib)
		FreeImage_Unload(_dib);
	_dib = new_dib;
	_bHasChanged = TRUE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////
// Creation & Destruction

fipImage::fipImage(FREE_IMAGE_TYPE image_type, unsigned width, unsigned height, unsigned bpp) {
	_dib = NULL;
	_bHasChanged = FALSE;
	if(width && height && bpp)
		setSize(image_type, width, height, bpp);
}

fipImage::~fipImage() {
	if(_dib) {
		FreeImage_Unload(_dib);
		_dib = NULL;
	}
}

FI_BOOL fipImage::setSize(FREE_IMAGE_TYPE image_type, unsigned width, unsigned height, unsigned bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	if(_dib) {
		FreeImage_Unload(_dib);
	}
	if((_dib = FreeImage_AllocateT(image_type, width, height, bpp, red_mask, green_mask, blue_mask)) == NULL)
		return FALSE;

	if(image_type == FIT_BITMAP) {
		// Create palette if needed
		switch(bpp)	{
			case 1:
			case 4:
			case 8:
				RGBQUAD *pal = FreeImage_GetPalette(_dib);
				for(unsigned i = 0; i < FreeImage_GetColorsUsed(_dib); i++) {
					pal[i].rgbRed = i;
					pal[i].rgbGreen = i;
					pal[i].rgbBlue = i;
				}
				break;
		}
	}

	_bHasChanged = TRUE;

	return TRUE;
}

void fipImage::clear() {
	if(_dib) {
		FreeImage_Unload(_dib);
		_dib = NULL;
	}
	_bHasChanged = TRUE;
}

///////////////////////////////////////////////////////////////////
// Copying

fipImage::fipImage(const fipImage& Image) {
	_dib = NULL;
	_fif = FIF_UNKNOWN;
	FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)Image._dib);
	replace(clone);
}

fipImage& fipImage::operator=(const fipImage& Image) {
	if(this != &Image) {
		FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)Image._dib);
		replace(clone);
	}
	return *this;
}

fipImage& fipImage::operator=(FIBITMAP *dib) {
	if(_dib != dib) {
		replace(dib);
	}
	return *this;
}

FI_BOOL fipImage::copySubImage(fipImage& dst, int left, int top, int right, int bottom) const {
	if(_dib) {
		dst = FreeImage_Copy(_dib, left, top, right, bottom);
		return dst.isValid();
	}
	return FALSE;
}

FI_BOOL fipImage::pasteSubImage(fipImage& src, int left, int top, int alpha) {
	if(_dib) {
		FI_BOOL bResult = FreeImage_Paste(_dib, src._dib, left, top, alpha);
		_bHasChanged = TRUE;
		return bResult;
	}
	return FALSE;
}

FI_BOOL fipImage::crop(int left, int top, int right, int bottom) {
	if(_dib) {
		FIBITMAP *dst = FreeImage_Copy(_dib, left, top, right, bottom);
		return replace(dst);
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////
// Information functions

FREE_IMAGE_TYPE fipImage::getImageType() const {
	return FreeImage_GetImageType(_dib);
}

unsigned fipImage::getWidth() const {
	return FreeImage_GetWidth(_dib); 
}

unsigned fipImage::getHeight() const {
	return FreeImage_GetHeight(_dib); 
}

unsigned fipImage::getScanWidth() const {
	return FreeImage_GetPitch(_dib);
}

FI_BOOL fipImage::isValid() const {
	return (_dib != NULL) ? TRUE:FALSE;
}

BITMAPINFO* fipImage::getInfo() const {
	return FreeImage_GetInfo(_dib);
}

BITMAPINFOHEADER* fipImage::getInfoHeader() const {
	return FreeImage_GetInfoHeader(_dib);
}

unsigned fipImage::getImageSize() const {
	return FreeImage_GetDIBSize(_dib);
}

unsigned fipImage::getImageMemorySize() const {
	return FreeImage_GetMemorySize(_dib);
}

unsigned fipImage::getBitsPerPixel() const {
	return FreeImage_GetBPP(_dib);
}

unsigned fipImage::getLine() const {
	return FreeImage_GetLine(_dib);
}

double fipImage::getHorizontalResolution() const {
	return (FreeImage_GetDotsPerMeterX(_dib) / (double)100); 
}

double fipImage::getVerticalResolution() const {
	return (FreeImage_GetDotsPerMeterY(_dib) / (double)100);
}

void fipImage::setHorizontalResolution(double value) {
	FreeImage_SetDotsPerMeterX(_dib, (unsigned)(value * 100 + 0.5));
}

void fipImage::setVerticalResolution(double value) {
	FreeImage_SetDotsPerMeterY(_dib, (unsigned)(value * 100 + 0.5));
}


///////////////////////////////////////////////////////////////////
// Palette operations

RGBQUAD* fipImage::getPalette() const {
	return FreeImage_GetPalette(_dib);
}

unsigned fipImage::getPaletteSize() const {
	return FreeImage_GetColorsUsed(_dib) * sizeof(RGBQUAD);
}

unsigned fipImage::getColorsUsed() const {
	return FreeImage_GetColorsUsed(_dib);
}

FREE_IMAGE_COLOR_TYPE fipImage::getColorType() const { 
	return FreeImage_GetColorType(_dib);
}

FI_BOOL fipImage::isGrayscale() const {
	return ((FreeImage_GetBPP(_dib) == 8) && (FreeImage_GetColorType(_dib) != FIC_PALETTE)); 
}

///////////////////////////////////////////////////////////////////
// Thumbnail access

FI_BOOL fipImage::getThumbnail(fipImage& image) const {
	image = FreeImage_Clone( FreeImage_GetThumbnail(_dib) );
	return image.isValid();
}

FI_BOOL fipImage::setThumbnail(const fipImage& image) {
	return FreeImage_SetThumbnail(_dib, (FIBITMAP*)image._dib);
}

FI_BOOL fipImage::hasThumbnail() const {
	return (FreeImage_GetThumbnail(_dib) != NULL);
}

FI_BOOL fipImage::clearThumbnail() {
	return FreeImage_SetThumbnail(_dib, NULL);
}


///////////////////////////////////////////////////////////////////
// Pixel access

BYTE* fipImage::accessPixels() const {
	return FreeImage_GetBits(_dib); 
}

BYTE* fipImage::getScanLine(unsigned scanline) const {
	if(scanline < FreeImage_GetHeight(_dib)) {
		return FreeImage_GetScanLine(_dib, scanline);
	}
	return NULL;
}

FI_BOOL fipImage::getPixelIndex(unsigned x, unsigned y, BYTE *value) const {
	return FreeImage_GetPixelIndex(_dib, x, y, value);
}

FI_BOOL fipImage::getPixelColor(unsigned x, unsigned y, RGBQUAD *value) const {
	return FreeImage_GetPixelColor(_dib, x, y, value);
}

FI_BOOL fipImage::setPixelIndex(unsigned x, unsigned y, BYTE *value) {
	_bHasChanged = TRUE;
	return FreeImage_SetPixelIndex(_dib, x, y, value);
}

FI_BOOL fipImage::setPixelColor(unsigned x, unsigned y, RGBQUAD *value) {
	_bHasChanged = TRUE;
	return FreeImage_SetPixelColor(_dib, x, y, value);
}

///////////////////////////////////////////////////////////////////
// File type identification

FREE_IMAGE_FORMAT fipImage::identifyFIF(const char* lpszPathName) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(lpszPathName, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}

	return fif;
}

FREE_IMAGE_FORMAT fipImage::identifyFIFU(const wchar_t* lpszPathName) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileTypeU(lpszPathName, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilenameU(lpszPathName);
	}

	return fif;
}

FREE_IMAGE_FORMAT fipImage::identifyFIFFromHandle(FreeImageIO *io, fi_handle handle) {
	if(io && handle) {
		// check the file signature and get its format
		return FreeImage_GetFileTypeFromHandle(io, handle, 16);
	}
	return FIF_UNKNOWN;
}

FREE_IMAGE_FORMAT fipImage::identifyFIFFromMemory(FIMEMORY *hmem) {
	if(hmem != NULL) {
		return FreeImage_GetFileTypeFromMemory(hmem, 0);
	}
	return FIF_UNKNOWN;
}


///////////////////////////////////////////////////////////////////
// Loading & Saving

FI_BOOL fipImage::load(const char* lpszPathName, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(lpszPathName, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(_dib) {
			FreeImage_Unload(_dib);			
		}
		// Load the file
		_dib = FreeImage_Load(fif, lpszPathName, flag);
		_bHasChanged = TRUE;
		if(_dib == NULL)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

FI_BOOL fipImage::loadU(const wchar_t* lpszPathName, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileTypeU(lpszPathName, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilenameU(lpszPathName);
	}
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(_dib) {
			FreeImage_Unload(_dib);			
		}
		// Load the file
		_dib = FreeImage_LoadU(fif, lpszPathName, flag);
		_bHasChanged = TRUE;
		if(_dib == NULL)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

FI_BOOL fipImage::loadFromHandle(FreeImageIO *io, fi_handle handle, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	fif = FreeImage_GetFileTypeFromHandle(io, handle, 16);
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(_dib) {
			FreeImage_Unload(_dib);			
		}
		// Load the file
		_dib = FreeImage_LoadFromHandle(fif, io, handle, flag);
		_bHasChanged = TRUE;
		if(_dib == NULL)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

FI_BOOL fipImage::loadFromMemory(fipMemoryIO& memIO, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	fif = memIO.getFileType();
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(_dib) {
			FreeImage_Unload(_dib);			
		}
		// Load the file
		_dib = memIO.load(fif, flag);
		_bHasChanged = TRUE;
		if(_dib == NULL)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

FI_BOOL fipImage::save(const char* lpszPathName, int flag) const {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FI_BOOL bSuccess = FALSE;

	// Try to guess the file format from the file extension
	fif = FreeImage_GetFIFFromFilename(lpszPathName);
	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		FI_BOOL bCanSave;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(_dib);
		if(image_type == FIT_BITMAP) {
			// standard bitmap type
			WORD bpp = FreeImage_GetBPP(_dib);
			bCanSave = (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp));
		} else {
			// special bitmap type
			bCanSave = FreeImage_FIFSupportsExportType(fif, image_type);
		}

		if(bCanSave) {
			bSuccess = FreeImage_Save(fif, _dib, lpszPathName, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

FI_BOOL fipImage::saveU(const wchar_t* lpszPathName, int flag) const {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FI_BOOL bSuccess = FALSE;

	// Try to guess the file format from the file extension
	fif = FreeImage_GetFIFFromFilenameU(lpszPathName);
	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		FI_BOOL bCanSave;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(_dib);
		if(image_type == FIT_BITMAP) {
			// standard bitmap type
			WORD bpp = FreeImage_GetBPP(_dib);
			bCanSave = (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp));
		} else {
			// special bitmap type
			bCanSave = FreeImage_FIFSupportsExportType(fif, image_type);
		}

		if(bCanSave) {
			bSuccess = FreeImage_SaveU(fif, _dib, lpszPathName, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

FI_BOOL fipImage::saveToHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flag) const {
	FI_BOOL bSuccess = FALSE;

	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		FI_BOOL bCanSave;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(_dib);
		if(image_type == FIT_BITMAP) {
			// standard bitmap type
			WORD bpp = FreeImage_GetBPP(_dib);
			bCanSave = (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp));
		} else {
			// special bitmap type
			bCanSave = FreeImage_FIFSupportsExportType(fif, image_type);
		}

		if(bCanSave) {
			bSuccess = FreeImage_SaveToHandle(fif, _dib, io, handle, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

FI_BOOL fipImage::saveToMemory(FREE_IMAGE_FORMAT fif, fipMemoryIO& memIO, int flag) const {
	FI_BOOL bSuccess = FALSE;

	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		FI_BOOL bCanSave;

		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(_dib);
		if(image_type == FIT_BITMAP) {
			// standard bitmap type
			WORD bpp = FreeImage_GetBPP(_dib);
			bCanSave = (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp));
		} else {
			// special bitmap type
			bCanSave = FreeImage_FIFSupportsExportType(fif, image_type);
		}

		if(bCanSave) {
			bSuccess = memIO.save(fif, _dib, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

///////////////////////////////////////////////////////////////////   
// Conversion routines

FI_BOOL fipImage::convertToType(FREE_IMAGE_TYPE image_type, FI_BOOL scale_linear) {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToType(_dib, image_type, scale_linear);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::threshold(BYTE T) {
	if(_dib) {
		FIBITMAP *dib1 = FreeImage_Threshold(_dib, T);
		return replace(dib1);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo4Bits() {
	if(_dib) {
		FIBITMAP *dib4 = FreeImage_ConvertTo4Bits(_dib);
		return replace(dib4);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo8Bits() {
	if(_dib) {
		FIBITMAP *dib8 = FreeImage_ConvertTo8Bits(_dib);
		return replace(dib8);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo16Bits555() {
	if(_dib) {
		FIBITMAP *dib16_555 = FreeImage_ConvertTo16Bits555(_dib);
		return replace(dib16_555);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo16Bits565() {
	if(_dib) {
		FIBITMAP *dib16_565 = FreeImage_ConvertTo16Bits565(_dib);
		return replace(dib16_565);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo24Bits() {
	if(_dib) {
		FIBITMAP *dibRGB = FreeImage_ConvertTo24Bits(_dib);
		return replace(dibRGB);
	}
	return FALSE;
}

FI_BOOL fipImage::convertTo32Bits() {
	if(_dib) {
		FIBITMAP *dib32 = FreeImage_ConvertTo32Bits(_dib);
		return replace(dib32);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToGrayscale() {
	if(_dib) {
		FIBITMAP *dib8 = FreeImage_ConvertToGreyscale(_dib);
		return replace(dib8);
	}
	return FALSE;
}

FI_BOOL fipImage::colorQuantize(FREE_IMAGE_QUANTIZE algorithm) {
	if(_dib) {
		FIBITMAP *dib8 = FreeImage_ColorQuantize(_dib, algorithm);
		return replace(dib8);
	}
	return FALSE;
}

FI_BOOL fipImage::dither(FREE_IMAGE_DITHER algorithm) {
	if(_dib) {
		FIBITMAP *dib = FreeImage_Dither(_dib, algorithm);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToFloat() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToFloat(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToRGBF() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToRGBF(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToRGBAF() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToRGBAF(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToUINT16() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToUINT16(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToRGB16() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToRGB16(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::convertToRGBA16() {
	if(_dib) {
		FIBITMAP *dib = FreeImage_ConvertToRGBA16(_dib);
		return replace(dib);
	}
	return FALSE;
}

FI_BOOL fipImage::toneMapping(FREE_IMAGE_TMO tmo, double first_param, double second_param, double third_param, double fourth_param) {
	if(_dib) {
		FIBITMAP *dst = NULL;
		// Apply a tone mapping algorithm and convert to 24-bit 
		switch(tmo) {
			case FITMO_REINHARD05:
				dst = FreeImage_TmoReinhard05Ex(_dib, first_param, second_param, third_param, fourth_param);
				break;
			default:
				dst = FreeImage_ToneMapping(_dib, tmo, first_param, second_param);
				break;
		}

		return replace(dst);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Transparency support: background colour and alpha channel

FI_BOOL fipImage::isTransparent() const {
	return FreeImage_IsTransparent(_dib);
}

unsigned fipImage::getTransparencyCount() const {
	return FreeImage_GetTransparencyCount(_dib);
}

BYTE* fipImage::getTransparencyTable() const {
	return FreeImage_GetTransparencyTable(_dib);
}

void fipImage::setTransparencyTable(BYTE *table, int count) {
	FreeImage_SetTransparencyTable(_dib, table, count);
	_bHasChanged = TRUE;
}

FI_BOOL fipImage::hasFileBkColor() const {
	return FreeImage_HasBackgroundColor(_dib);
}

FI_BOOL fipImage::getFileBkColor(RGBQUAD *bkcolor) const {
	return FreeImage_GetBackgroundColor(_dib, bkcolor);
}

FI_BOOL fipImage::setFileBkColor(RGBQUAD *bkcolor) {
	_bHasChanged = TRUE;
	return FreeImage_SetBackgroundColor(_dib, bkcolor);
}

///////////////////////////////////////////////////////////////////   
// Channel processing support

FI_BOOL fipImage::getChannel(fipImage& image, FREE_IMAGE_COLOR_CHANNEL channel) const {
	if(_dib) {
		image = FreeImage_GetChannel(_dib, channel);
		return image.isValid();
	}
	return FALSE;
}

FI_BOOL fipImage::setChannel(fipImage& image, FREE_IMAGE_COLOR_CHANNEL channel) {
	if(_dib) {
		_bHasChanged = TRUE;
		return FreeImage_SetChannel(_dib, image._dib, channel);
	}
	return FALSE;
}

FI_BOOL fipImage::splitChannels(fipImage& RedChannel, fipImage& GreenChannel, fipImage& BlueChannel) {
	if(_dib) {
		RedChannel = FreeImage_GetChannel(_dib, FICC_RED);
		GreenChannel = FreeImage_GetChannel(_dib, FICC_GREEN);
		BlueChannel = FreeImage_GetChannel(_dib, FICC_BLUE);

		return (RedChannel.isValid() && GreenChannel.isValid() && BlueChannel.isValid());
	}
	return FALSE;
}

FI_BOOL fipImage::combineChannels(fipImage& red, fipImage& green, fipImage& blue) {
	if(!_dib) {
		int width = red.getWidth();
		int height = red.getHeight();
		_dib = FreeImage_Allocate(width, height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	}

	if(_dib) {
		FI_BOOL bResult = TRUE;
		bResult &= FreeImage_SetChannel(_dib, red._dib, FICC_RED);
		bResult &= FreeImage_SetChannel(_dib, green._dib, FICC_GREEN);
		bResult &= FreeImage_SetChannel(_dib, blue._dib, FICC_BLUE);

		_bHasChanged = TRUE;

		return bResult;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Rotation and flipping

FI_BOOL fipImage::rotateEx(double angle, double x_shift, double y_shift, double x_origin, double y_origin, FI_BOOL use_mask) {
	if(_dib) {
		if(FreeImage_GetBPP(_dib) >= 8) {
			FIBITMAP *rotated = FreeImage_RotateEx(_dib, angle, x_shift, y_shift, x_origin, y_origin, use_mask);
			return replace(rotated);
		}
	}
	return FALSE;
}

FI_BOOL fipImage::rotate(double angle, const void *bkcolor) {
	if(_dib) {
		switch(FreeImage_GetImageType(_dib)) {
			case FIT_BITMAP:
				switch(FreeImage_GetBPP(_dib)) {
					case 1:
					case 8:
					case 24:
					case 32:
						break;
					default:
						return FALSE;
				}
				break;

			case FIT_UINT16:
			case FIT_RGB16:
			case FIT_RGBA16:
			case FIT_FLOAT:
			case FIT_RGBF:
			case FIT_RGBAF:
				break;
			default:
				return FALSE;
				break;
		}

		FIBITMAP *rotated = FreeImage_Rotate(_dib, angle, bkcolor);
		return replace(rotated);

	}
	return FALSE;
}

FI_BOOL fipImage::flipVertical() {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_FlipVertical(_dib);
	}
	return FALSE;
}

FI_BOOL fipImage::flipHorizontal() {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_FlipHorizontal(_dib);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Color manipulation routines

FI_BOOL fipImage::invert() {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_Invert(_dib);
	}
	return FALSE;
}

FI_BOOL fipImage::adjustCurve(BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel) {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_AdjustCurve(_dib, LUT, channel);
	}
	return FALSE;
}

FI_BOOL fipImage::adjustGamma(double gamma) {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_AdjustGamma(_dib, gamma);
	}
	return FALSE;
}

FI_BOOL fipImage::adjustBrightness(double percentage) {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_AdjustBrightness(_dib, percentage);
	}
	return FALSE;
}

FI_BOOL fipImage::adjustContrast(double percentage) {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_AdjustContrast(_dib, percentage);
	}
	return FALSE;
}

FI_BOOL fipImage::adjustBrightnessContrastGamma(double brightness, double contrast, double gamma) {
	if(_dib) {
		_bHasChanged = TRUE;

		return FreeImage_AdjustColors(_dib, brightness, contrast, gamma, FALSE);
	}
	return FALSE;
}

FI_BOOL fipImage::getHistogram(DWORD *histo, FREE_IMAGE_COLOR_CHANNEL channel) const {
	if(_dib) {
		return FreeImage_GetHistogram(_dib, histo, channel);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////
// Upsampling / downsampling routine

FI_BOOL fipImage::rescale(unsigned new_width, unsigned new_height, FREE_IMAGE_FILTER filter) {
	if(_dib) {
		switch(FreeImage_GetImageType(_dib)) {
			case FIT_BITMAP:
			case FIT_UINT16:
			case FIT_RGB16:
			case FIT_RGBA16:
			case FIT_FLOAT:
			case FIT_RGBF:
			case FIT_RGBAF:
				break;
			default:
				return FALSE;
				break;
		}

		// Perform upsampling / downsampling
		FIBITMAP *dst = FreeImage_Rescale(_dib, new_width, new_height, filter);
		return replace(dst);
	}
	return FALSE;
}

FI_BOOL fipImage::makeThumbnail(unsigned max_size, FI_BOOL convert) {
	if(_dib) {
		switch(FreeImage_GetImageType(_dib)) {
			case FIT_BITMAP:
			case FIT_UINT16:
			case FIT_RGB16:
			case FIT_RGBA16:
			case FIT_FLOAT:
			case FIT_RGBF:
			case FIT_RGBAF:
				break;
			default:
				return FALSE;
				break;
		}

		// Perform downsampling
		FIBITMAP *dst = FreeImage_MakeThumbnail(_dib, max_size, convert);
		return replace(dst);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////
// Metadata

unsigned fipImage::getMetadataCount(FREE_IMAGE_MDMODEL model) const {
	return FreeImage_GetMetadataCount(model, _dib);
}

FI_BOOL fipImage::getMetadata(FREE_IMAGE_MDMODEL model, const char *key, fipTag& tag) const {
	FITAG *searchedTag = NULL;
	FreeImage_GetMetadata(model, _dib, key, &searchedTag);
	if(searchedTag != NULL) {
		tag = FreeImage_CloneTag(searchedTag);
		return TRUE;
	} else {
		// clear the tag
		tag = (FITAG*)NULL;
	}
	return FALSE;
}

FI_BOOL fipImage::setMetadata(FREE_IMAGE_MDMODEL model, const char *key, fipTag& tag) {
	return FreeImage_SetMetadata(model, _dib, key, tag);
}

