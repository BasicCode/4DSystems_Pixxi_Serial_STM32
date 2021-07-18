/**
 * Serial library for 4D Systems Pixxi based displays.
 * See CPP file for full description.
 *
 */
#ifndef Pixxi_Serial_4DLib_h
#define Pixxi_Serial_4DLib_h

/*
 * Using constants available from 4D Systems, OR provide your own.
 * https://github.com/4dsystems/Pixxi-Serial-Arduino-Library/
 */
#include "stm32l4xx_hal.h"
#include "Pixxi_Const4D.h"
#include <string.h>

typedef void (*Tcallback4D)(int, unsigned char);

class Pixxi_Serial_4DLib
{
	public:
		Pixxi_Serial_4DLib(UART_HandleTypeDef * virtualPort);
		Tcallback4D Callback4D ;

		//STM specific routines
		void WriteInt(uint16_t data);

		//Compound 4D Routines
		uint16_t bus_In();
		void bus_Out(uint16_t Bits);
		uint16_t bus_Read();
		void bus_Set(uint16_t IOMap);
		void bus_Write(uint16_t Bits);
		uint16_t charheight(char  TestChar);
		uint16_t charwidth(char  TestChar);
		uint16_t file_Close(uint16_t  Handle);
		uint16_t file_Count(char *  Filename);
		uint16_t file_Dir(char *  Filename);
		uint16_t file_Erase(char *  Filename);
		uint16_t file_Error();
		uint16_t file_Exec(char *  Filename, uint16_t  ArgCount, t4DWordArray  Args);
		uint16_t file_Exists(char *  Filename);
		uint16_t file_FindFirst(char *  Filename);
		uint16_t file_FindNext();
		char file_GetC(uint16_t  Handle);
		uint16_t file_GetS(char *  StringIn, uint16_t  Size, uint16_t  Handle);
		uint16_t file_GetW(uint16_t  Handle);
		uint16_t file_Image(uint16_t  X, uint16_t  Y, uint16_t  Handle);
		uint16_t file_Index(uint16_t  Handle, uint16_t  HiSize, uint16_t  LoSize, uint16_t  Recordnum);
		uint16_t file_LoadFunction(char *  Filename);
		uint16_t file_LoadImageControl(char *  Datname, char *  GCIName, uint16_t  Mode);
		uint16_t file_LoadImageControl(uint16_t highOffset, uint16_t  lowOffset, uint16_t Mode);
		uint16_t file_LoadImageControl(int highOffset, int  lowOffset, int Mode);
		uint16_t file_Mount();
		uint16_t file_Open(char *  Filename, char  Mode);
		uint16_t file_PlayWAV(char *  Filename);
		uint16_t file_PutC(char  Character, uint16_t  Handle);
		uint16_t file_PutS(char *  StringOut, uint16_t  Handle);
		uint16_t file_PutW(uint16_t  Word, uint16_t  Handle);
		uint16_t file_Read(t4DByteArray  Data, uint16_t  Size, uint16_t  Handle);
		uint16_t file_Rewind(uint16_t  Handle);
		uint16_t file_Run(char *  Filename, uint16_t  ArgCount, t4DWordArray  Args);
		uint16_t file_ScreenCapture(uint16_t  X, uint16_t  Y, uint16_t  Width, uint16_t  Height, uint16_t  Handle);
		uint16_t file_Seek(uint16_t  Handle, uint16_t  HiWord, uint16_t  LoWord);
		uint16_t file_Size(uint16_t  Handle, uint16_t *  HiWord, uint16_t *  LoWord);
		uint16_t file_Tell(uint16_t  Handle, uint16_t *  HiWord, uint16_t *  LoWord);
		void file_Unmount();
		uint16_t file_Write(uint16_t  Size, t4DByteArray  Source, uint16_t  Handle);
		uint16_t gfx_BevelShadow(uint16_t  Value);
		uint16_t gfx_BevelWidth(uint16_t  Value);
		uint16_t gfx_BGcolour(uint16_t  Color);
		void gfx_Button(uint16_t  Up, uint16_t  x, uint16_t  y, uint16_t  buttonColour, uint16_t  txtColour, uint16_t  font, uint16_t  txtWidth, uint16_t  txtHeight, char *   text);
		void gfx_ChangeColour(uint16_t  OldColor, uint16_t  NewColor);
		void gfx_Circle(uint16_t  X, uint16_t  Y, uint16_t  Radius, uint16_t  Color);
		void gfx_CircleFilled(uint16_t  X, uint16_t  Y, uint16_t  Radius, uint16_t  Color);
		void gfx_Clipping(uint16_t  OnOff);
		void gfx_ClipWindow(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2);
		void gfx_Cls();
		uint16_t gfx_Contrast(uint16_t  Contrast);
		void gfx_Ellipse(uint16_t  X, uint16_t  Y, uint16_t  Xrad, uint16_t  Yrad, uint16_t  Color);
		void gfx_EllipseFilled(uint16_t  X, uint16_t  Y, uint16_t  Xrad, uint16_t  Yrad, uint16_t  Color);
		uint16_t gfx_FrameDelay(uint16_t  Msec);
		uint16_t gfx_Get(uint16_t  Mode);
		uint16_t gfx_GetPixel(uint16_t  X, uint16_t  Y);
		void gfx_Line(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
		uint16_t gfx_LinePattern(uint16_t  Pattern);
		void gfx_LineTo(uint16_t  X, uint16_t  Y);
		void gfx_MoveTo(uint16_t  X, uint16_t  Y);
		uint16_t gfx_Orbit(uint16_t  Angle, uint16_t  Distance, uint16_t *  Xdest, uint16_t *  Ydest);
		uint16_t gfx_OutlineColour(uint16_t  Color);
		void gfx_Panel(uint16_t  Raised, uint16_t  X, uint16_t  Y, uint16_t  Width, uint16_t  Height, uint16_t  Color);
		void gfx_Polygon(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  Color);
		void gfx_PolygonFilled(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  Color);
		void gfx_Polyline(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  Color);
		void gfx_PutPixel(uint16_t  X, uint16_t  Y, uint16_t  Color);
		void gfx_Rectangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
		void gfx_RectangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
		void gfx_ScreenCopyPaste(uint16_t  Xs, uint16_t  Ys, uint16_t  Xd, uint16_t  Yd, uint16_t  Width, uint16_t  Height);
		uint16_t gfx_ScreenMode(uint16_t  ScreenMode);
		void gfx_Set(uint16_t  Func, uint16_t  Value);
		void gfx_SetClipRegion();
		uint16_t gfx_Slider(uint16_t  Mode, uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color, uint16_t  Scale, uint16_t  Value);
		uint16_t gfx_Transparency(uint16_t  OnOff);
		uint16_t gfx_TransparentColour(uint16_t  Color);
		void gfx_Triangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  Color);
		void gfx_TriangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  Color);
		void gfx_Scale(uint16_t hndl, uint16_t params);
		void gfx_Panel2(uint16_t options, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t offset1, uint16_t offset2, uint16_t mainColor, uint16_t shadowColor, uint16_t fillColor);
		void gfx_Button4(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_Switch(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_Slider5(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_Dial(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_Led(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_Gauge(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_AngularMeter(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_LedDigit(uint16_t x, uint16_t y, uint16_t digitSize, uint16_t onColour, uint16_t offColour, uint16_t value);
		void gfx_RulerGauge(uint16_t value, uint16_t hndl, uint16_t params);
		void gfx_LedDigits(uint16_t value, uint16_t hndl, uint16_t params);
		uint16_t img_ClearAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value);
		uint16_t img_Darken(uint16_t  Handle, uint16_t  Index);
		uint16_t img_Disable(uint16_t  Handle, uint16_t  Index);
		uint16_t img_Enable(uint16_t  Handle, uint16_t  Index);
		uint16_t img_GetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset );
		uint16_t img_Lighten(uint16_t  Handle, uint16_t  Index);
		uint16_t img_SetAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value);
		uint16_t img_SetPosition(uint16_t  Handle, uint16_t  Index, uint16_t  Xpos, uint16_t  Ypos);
		uint16_t img_SetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset , uint16_t  Word);
		uint16_t img_Show(uint16_t  Handle, uint16_t  Index);
		uint16_t img_Touched(uint16_t  Handle, uint16_t  Index);
		void img_FunctionCall(uint16_t imgHndl, uint16_t index, uint16_t value, uint16_t hndl, uint16_t params, uint16_t argCount, uint16_t strMap);
		uint16_t media_Flush();
		void media_Image(uint16_t  X, uint16_t  Y);
		uint16_t media_Init();
		uint16_t media_RdSector(t4DSector  SectorIn);
		uint16_t media_ReadByte();
		uint16_t media_ReadWord();
		void media_SetAdd(uint16_t  HiWord, uint16_t  LoWord);
		void media_SetSector(uint16_t  HiWord, uint16_t  LoWord);
		void media_Video(uint16_t  X, uint16_t  Y);
		void media_VideoFrame(uint16_t  X, uint16_t  Y, uint16_t  Framenumber);
		uint16_t media_WriteByte(uint16_t  Byte);
		uint16_t media_WriteWord(uint16_t  Word);
		uint16_t media_WrSector(t4DSector  SectorOut);
		uint16_t mem_Alloc(uint16_t size);
		uint16_t mem_Free(uint16_t  Handle);
		uint16_t mem_Heap();
		uint16_t pin_HI(uint16_t Pin);
		uint16_t peekM(uint16_t  Address);
		uint16_t pin_LO(uint16_t Pin);
		uint16_t pin_Read(uint16_t Pin);
		uint16_t pin_Set(uint16_t Mode, uint16_t Pin);
        void putCH(uint16_t  WordChar);
		void pokeM(uint16_t  Address, uint16_t  WordValue) ;
		uint16_t putstr(char *  InString);
		/**
		 * This orginal library includes a number of conversion functions
		 * which I have chosen not to implement here out of laziness.
		 */
		void print(const char[]);
		void println(const char[]);

		void snd_BufSize(uint16_t  Bufsize);
		void snd_Continue();
		void snd_Pause();
		uint16_t snd_Pitch(uint16_t  Pitch);
		uint16_t snd_Playing();
		void snd_Stop();
		void snd_Volume(uint16_t  Volume);
		uint16_t str_Ptr(uint16_t buffer);
		uint16_t sys_Sleep(uint16_t  Units);
		void touch_DetectRegion(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2);
		uint16_t touch_Get(uint16_t  Mode);
		void touch_Set(uint16_t  Mode);
		uint16_t txt_Attributes(uint16_t  Attribs);
		uint16_t txt_BGcolour(uint16_t  Color);
		uint16_t txt_Bold(uint16_t  Bold);
		uint16_t txt_FGcolour(uint16_t  Color);
		uint16_t txt_FontID(uint16_t  FontNumber);
		uint16_t txt_Height(uint16_t  Multiplier);
		uint16_t txt_Inverse(uint16_t  Inverse);
		uint16_t txt_Italic(uint16_t  Italic);
		void txt_MoveCursor(uint16_t  Line, uint16_t  Column);
		uint16_t txt_Opacity(uint16_t  TransparentOpaque);
		void txt_Set(uint16_t  Func, uint16_t  Value);
		uint16_t txt_Underline(uint16_t  Underline);
		uint16_t txt_Width(uint16_t  Multiplier);
		uint16_t txt_Wrap(uint16_t  Position);
		uint16_t txt_Xgap(uint16_t  Pixels);
		uint16_t txt_Ygap(uint16_t  Pixels);
		uint16_t file_CallFunction(uint16_t  Handle, uint16_t  ArgCount, t4DWordArray  Args);
		uint16_t sys_GetModel(char *  ModelStr);
		uint16_t sys_GetVersion();
		uint16_t sys_GetPmmC();
		uint16_t writeString(uint16_t  Handle, char *  StringOut);
		uint16_t readString(uint16_t  Handle, char *  StringIn);
		void blitComtoDisplay(uint16_t  X, uint16_t  Y, uint16_t  Width, uint16_t  Height, t4DByteArray  Pixels);
		void SendWordArrayToRAM(uint16_t  hndl, uint16_t  length, uint16_t * data);
		void SendByteArrayToRAM(uint16_t  hndl, uint16_t  length, char * data);
		uint16_t file_FindFirstRet(char *  Filename, char *  StringIn);
		uint16_t file_FindNextRet(char *  StringIn);
		void setbaudWait(uint16_t  Newrate);
		uint16_t widget_Create(uint16_t count);
		void widget_Add(uint16_t hndl, uint16_t index, uint16_t widget);
		void widget_Delete(uint16_t hndl, uint16_t index);
		uint16_t widget_Realloc(uint16_t hndl, uint16_t count);
		uint16_t widget_SetWord(uint16_t Handle, uint16_t  Index, uint16_t  Offset , uint16_t  Word);
		uint16_t widget_GetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset );
		uint16_t widget_SetPosition(uint16_t  Handle, uint16_t  Index, uint16_t  Xpos, uint16_t  Ypos);
		uint16_t widget_Enable(uint16_t  Handle, uint16_t  Index);
		uint16_t widget_Disable(uint16_t  Handle, uint16_t  Index);
		uint16_t widget_SetAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value);
		uint16_t widget_ClearAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value);
		uint16_t widget_Touched(uint16_t  Handle, uint16_t  Index);
		void widget_InitGradRAM(uint16_t hndl);
		uint16_t widget_InitString(char * str);
		uint16_t widget_InitStringPtr(char * str);
		uint16_t widget_InitStringArray(char * str, uint16_t len);
		void widget_Init(uint16_t len, uint16_t * data, uint16_t * hndl, uint16_t * param);

		void GetAck(void);

		//4D Global Variables Used
		int Error4D;  				// Error indicator,  used and set by Intrinsic routines
		unsigned char Error4D_Inv;	// Error byte returned from com port, onl set if error = Err_Invalid
	//	int Error_Abort4D;  		// if true routines will abort when detecting an error

		/**
		 * After some testing, I found that 2000ms was not sufficient for the FLASH / SD card to initialise.
		 * 3000ms seems to do the trick.
		 */
		unsigned long TimeLimit4D = 3000;	// time limit in ms for total serial command duration, 2000 (2 seconds) should be adequate for most commands
									// assuming a reasonable baud rate AND low latency AND 0 for the Serial Delay Parameter
									// temporary increase might be required for very long (bitmap write, large image file opens)
									// or indeterminate (eg file_exec, file_run, file_callFunction) commands

	private:
		UART_HandleTypeDef * _huart;

		//Intrinsic 4D Routines
		void WriteChars(char * charsout);
		void WriteBytes(char * Source, int Size);
		void WriteWords(uint16_t * Source, int Size);
		void getbytes(char * data, int size);
		uint16_t GetWord(void);
		void getString(char * outStr, int strLen);
		uint16_t GetAckResp(void);
		uint16_t GetAckRes2Words(uint16_t * word1, uint16_t * word2);
		void GetAck2Words(uint16_t * word1, uint16_t * word2);
		uint16_t GetAckResSector(t4DSector Sector);
		uint16_t GetAckResStr(char * OutStr);
		uint16_t GetAckResData(t4DByteArray OutData, uint16_t size);
		void SetThisBaudrate(int Newrate);

		void printNumber(unsigned long, uint8_t);
		void printFloat(double number, uint8_t digits);
};


#endif
