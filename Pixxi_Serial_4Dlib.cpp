/**
 * Driver for 4D Systems Pixxi based displays.
 *
 * Designed and tested on PixxiLCD 39P4 display over UART at 115200 baud.
 *
 * Current State: Work-In-Progress. This is a direct port of the Pixxi-Serial library provided
 * by 4D Systems, available here:
 * https://github.com/4dsystems/Pixxi-Serial-Arduino-Library/
 *
 * You will also need to download the Pixxi_Const4D.h file from their GitHub which contains
 * all of the colours and commands.
 *
 * TODO: Include license.
 */

#include "stm32l4xx_hal.h"
#include <Pixxi_Serial_4Dlib.h>

Pixxi_Serial_4DLib::Pixxi_Serial_4DLib(UART_HandleTypeDef * port) {
	_huart = port;
	//Flush the buffer
	HAL_UART_AbortReceive(_huart);
}

/**
 * UART Methods
 */

/*
 * Generic method to write a (uint16_t) command to the display.
 */
void Pixxi_Serial_4DLib::WriteInt(uint16_t data) {
	//Separate the upper and lower bytes
	uint8_t thisData[] = {(uint8_t) (data >> 8), (uint8_t) (data & 0xFF)};
    //Write the data
	HAL_UART_Transmit(_huart, thisData, 2, 10);
}

//*********************************************************************************************//
//**********************************Intrinsic 4D Routines**************************************//
//*********************************************************************************************//

void Pixxi_Serial_4DLib::WriteChars(char * charsout)
{
	//Count how many bytes to write
	int numBytes = 0;
	while(*charsout++) {
		numBytes++;
		//Protect against overrun somewhat
		if(numBytes > 1000)
			break;
	}

	HAL_UART_Transmit(_huart, (uint8_t *) charsout, numBytes, 10) ;
}

void Pixxi_Serial_4DLib::WriteBytes(uint8_t * source, int size)
{
	HAL_UART_Transmit(_huart, source, size, 10);
}

void Pixxi_Serial_4DLib::WriteWords(uint16_t * Source, int Size)
{
  uint16_t wk ;
  for (int i = 0; i < Size; i++)
  {
    wk = *Source++ ;
	//Separate the upper and lower bytes
	uint8_t thisData[] = {(uint8_t) (wk >> 8), (uint8_t) (wk & 0xFF)};
    HAL_UART_Transmit(_huart, (uint8_t *) thisData, 2, 10) ;
  }
}

void Pixxi_Serial_4DLib::getbytes(uint8_t * data, int size)
{
	int response = HAL_UART_Receive(_huart, data, size, TimeLimit4D);

	if (response != HAL_OK)
	{
		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
	}
}

void Pixxi_Serial_4DLib::GetAck(void)
{
	uint8_t readx = 0;
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, &readx, 1, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
}

uint16_t Pixxi_Serial_4DLib::GetWord(void)
{
	uint8_t readx[2] = {0, 0};

	if (Error4D != Err4D_OK)
		return 0 ;

	//Receive 2 bytes
	int response = HAL_UART_Receive(_huart, readx, 2, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D  = Err4D_Timeout ;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
		return 0 ;
	}
	else
		return readx[0] << 8 | readx[1] ;
}

void Pixxi_Serial_4DLib::getString(char * outStr, int strLen)
{
	if (Error4D != Err4D_OK)
	{
		outStr[0] = 0 ;
		return ;
	}

	int response = HAL_UART_Receive(_huart, (uint8_t *) outStr, strLen, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D  = Err4D_Timeout ;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
	}

	//Add a terminator
	//TODO: Not sure if this is required?
	outStr[strLen - 1] = 0 ;
}

uint16_t Pixxi_Serial_4DLib::GetAckResp(void)
{
	uint8_t readx[3] = {0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 3, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	return (uint16_t) ((readx[1] << 8) | (readx[2] & 0xFF));
}

/*
 * TODO: These next five GET functions probably won't work and I have not tested them yet.
 * Suggest using the native HAL_Receive function with the specified number of bytes.
 */
uint16_t Pixxi_Serial_4DLib::GetAckRes2Words(uint16_t * word1, uint16_t * word2)
{

	uint8_t readx[7] = {0, 0, 0, 0, 0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 7, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	//Update the provided buffers
	*word1 = (readx[3] << 8) | (readx[4] & 0xFF);
	*word2 = (readx[5] << 8) | (readx[6] & 0xFF);

	//Return result code
	return (readx[1] << 8) | (readx[2] & 0xFF);
}

void Pixxi_Serial_4DLib::GetAck2Words(uint16_t * word1, uint16_t * word2)
{
	uint8_t readx[5] = {0, 0, 0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 5, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	//Update the provided buffers
	*word1 = (readx[1] << 8) | (readx[2] & 0xFF);
	*word2 = (readx[3] << 8) | (readx[4] & 0xFF);
}

//TODO: Fix this
uint16_t Pixxi_Serial_4DLib::GetAckResSector(uint8_t * Sector)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getbytes(Sector, 512) ;
	return Result ;
}

//TODO: Fix this
uint16_t Pixxi_Serial_4DLib::GetAckResStr(char * OutStr)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getString(OutStr, Result) ;
	return Result ;
}

//TODO: Fix this
uint16_t Pixxi_Serial_4DLib::GetAckResData(uint8_t * OutData, uint16_t size)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getbytes(OutData, size) ;
	return Result ;
}

void Pixxi_Serial_4DLib::SetThisBaudrate(int Newrate)
{
	//this probably isn't needed on this MCU, I dunno
}

//*********************************************************************************************//
//**********************************Compound 4D Routines***************************************//
//*********************************************************************************************//

uint16_t Pixxi_Serial_4DLib::bus_In()
{
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_In >> 8), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_In), 1, 10) ;
	return GetAckResp() ;
}

void Pixxi_Serial_4DLib::bus_Out(uint16_t bits)
{
	WriteInt(F_bus_Out);
	WriteInt(bits);
	GetAck() ;
}

uint16_t Pixxi_Serial_4DLib::bus_Read()
{
	WriteInt(F_bus_Read);
	return GetAckResp() ;
}

void Pixxi_Serial_4DLib::bus_Set(uint16_t IOMap)
{
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_Set >> 8), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_Set), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (IOMap >> 8), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (IOMap & 0xFF), 1, 10) ;
	GetAck() ;
}

void Pixxi_Serial_4DLib::bus_Write(uint16_t bits)
{
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_Write >> 8), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (F_bus_Write), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (bits >> 8), 1, 10) ;
	HAL_UART_Transmit(_huart, (uint8_t *) (bits & 0xFF), 1, 10) ;
	GetAck() ;
}

uint16_t Pixxi_Serial_4DLib::charheight(char  testChar)
{
	WriteInt(F_charheight);
	WriteBytes((uint8_t *) testChar, 1);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::charwidth(char testChar)
{
	WriteInt(F_charwidth);
	WriteBytes((uint8_t *) testChar, 1);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Close(uint16_t  handle)
{
	WriteInt(F_file_Close);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Count(char *  filename)
{
	WriteInt(F_file_Count);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Dir(char *  filename)
{
	WriteInt(F_file_Dir);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Erase(char *  filename)
{
	WriteInt(F_file_Erase);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Error()
{
	WriteInt(F_file_Error);

	return GetAckResp();
}


uint16_t Pixxi_Serial_4DLib::file_Exec(char *  filename, uint16_t  argCount, t4DWordArray  args)
{
	WriteInt(F_file_Exec);
	WriteChars(filename);
	WriteInt(argCount);
	WriteWords(args, argCount);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Exists(char *  filename)
{
	WriteInt(F_file_Exists);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_FindFirst(char *  filename)
{
	WriteInt(F_file_FindFirst);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_FindNext()
{
	WriteInt(F_file_FindNext);

	return GetAckResp();
}

char Pixxi_Serial_4DLib::file_GetC(uint16_t  handle)
{
	WriteInt(F_file_GetC);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_GetS(char *  stringIn, uint16_t  size, uint16_t  handle)
{
	WriteInt(F_file_GetS);
	WriteInt(size);
	WriteInt(handle);

	return GetAckResStr(stringIn);
}

uint16_t Pixxi_Serial_4DLib::file_GetW(uint16_t  handle)
{
	WriteInt(F_file_GetW);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Image(uint16_t  X, uint16_t  Y, uint16_t  handle)
{
	WriteInt(F_file_Image);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Index(uint16_t  handle, uint16_t  hiSize, uint16_t  loSize, uint16_t  recordNum)
{
	WriteInt(F_file_Index);
	WriteInt(handle);
	WriteInt(hiSize);
	WriteInt(loSize);
	WriteInt(recordNum);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_LoadFunction(char *  filename)
{
	WriteInt(F_file_LoadFunction);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_LoadImageControl(char *  datname, char *  GCIName, uint16_t  mode)
{
	WriteInt(F_file_LoadImageControl);
	WriteChars(datname);
	WriteChars(GCIName);
	WriteInt(mode);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_LoadImageControl(uint16_t hiOffset, uint16_t loOffset, uint16_t mode)
{
	WriteInt(F_file_LoadImageControl);
	WriteInt(hiOffset);
	WriteInt(loOffset);
	WriteInt(mode);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_LoadImageControl(int hiOffset, int loOffset, int mode)
{
  return file_LoadImageControl((uint16_t) hiOffset, (uint16_t) loOffset, (uint16_t) mode);
}

uint16_t Pixxi_Serial_4DLib::file_Mount()
{
	WriteInt(F_file_Mount);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Open(char *  filename, char  mode)
{
	WriteInt(F_file_Open);
	WriteChars(filename);
	WriteBytes((uint8_t *) mode, 1);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_PlayWAV(char *  filename)
{
	WriteInt(F_file_PlayWAV);
	WriteChars(filename);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_PutC(char  character, uint16_t  handle)
{
	WriteInt(F_file_PutC);
	WriteInt(character);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_PutS(char *  stringOut, uint16_t  handle)
{
	WriteInt(F_file_PutS);
	WriteChars(stringOut);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_PutW(uint16_t  word, uint16_t  handle)
{
	WriteInt(F_file_PutW);
	WriteInt(word);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Read(uint8_t *  data, uint16_t  size, uint16_t  handle)
{
	WriteInt(F_file_Read);
	WriteInt(size);
	WriteInt(handle);

	return GetAckResData(data,size);
}

uint16_t Pixxi_Serial_4DLib::file_Rewind(uint16_t  handle)
{
	WriteInt(F_file_Rewind);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Run(char *  filename, uint16_t  argCount, t4DWordArray  args)
{
	WriteInt(F_file_Run);
	WriteChars(filename);
	WriteInt(argCount);
	WriteWords(args, argCount);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_ScreenCapture(uint16_t  X, uint16_t  Y, uint16_t  width, uint16_t  height, uint16_t  handle)
{
	WriteInt(F_file_ScreenCapture);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(width);
	WriteInt(height);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Seek(uint16_t  handle, uint16_t  hiWord, uint16_t  loWord)
{
	WriteInt(F_file_Seek);
	WriteInt(handle);
	WriteInt(hiWord);
	WriteInt(loWord);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::file_Size(uint16_t  handle, uint16_t *  hiWord, uint16_t *  loWord)
{
	WriteInt(F_file_Size);
	WriteInt(handle);

	return GetAckRes2Words(hiWord, loWord);
}

uint16_t Pixxi_Serial_4DLib::file_Tell(uint16_t  handle, uint16_t *  hiWord, uint16_t *  loWord)
{
	WriteInt(F_file_Tell);
	WriteInt(handle);


	return GetAckRes2Words(hiWord, loWord);
}

void Pixxi_Serial_4DLib::file_Unmount()
{
	WriteInt(F_file_Unmount);
	GetAck();
}

uint16_t Pixxi_Serial_4DLib::file_Write(uint16_t  size, uint8_t * source, uint16_t  handle)
{
	WriteInt(F_file_Write);
	WriteInt(size);
	WriteBytes(source, size);
	WriteInt(handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_BevelShadow(uint16_t  value)
{
	WriteInt(F_gfx_BevelShadow);
	WriteInt(value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_BevelWidth(uint16_t  value)
{
	WriteInt(F_gfx_BevelWidth);
	WriteInt(value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_BGcolour(uint16_t  colour)
{
	WriteInt(F_gfx_BGcolour);
	WriteInt(colour);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Button(uint16_t  up, uint16_t  x, uint16_t  y, uint16_t  buttonColour, uint16_t  txtColour, uint16_t  font, uint16_t  txtWidth, uint16_t  txtHeight, char *   text)
{
	WriteInt(F_gfx_Button);
	WriteInt(up);
	WriteInt(x);
	WriteInt(y);
	WriteInt(buttonColour);
	WriteInt(txtColour);
	WriteInt(font);
	WriteInt(txtWidth);
	WriteInt(txtHeight);

	WriteChars(text);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_ChangeColour(uint16_t  oldColour, uint16_t  newColour)
{
	WriteInt(F_gfx_ChangeColour);
	WriteInt(oldColour);
	WriteInt(newColour);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Circle(uint16_t  X, uint16_t  Y, uint16_t  radius, uint16_t  colour)
{
	WriteInt(F_gfx_Circle);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(radius);
	WriteInt(colour);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_CircleFilled(uint16_t  X, uint16_t  Y, uint16_t  radius, uint16_t  colour)
{
	WriteInt(F_gfx_CircleFilled);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(radius);
	WriteInt(colour);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Clipping(uint16_t  onOff)
{
	WriteInt(F_gfx_Clipping);
	WriteInt(onOff);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_ClipWindow(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2)
{
	WriteInt(F_gfx_ClipWindow);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Cls()
{
	WriteInt(F_gfx_Cls);
	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_Contrast(uint16_t  Contrast)
{
	WriteInt(F_gfx_Contrast);
	WriteInt(Contrast);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Ellipse(uint16_t  X, uint16_t  Y, uint16_t  Xrad, uint16_t  Yrad, uint16_t  colour)
{
	WriteInt(F_gfx_Ellipse);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Xrad);
	WriteInt(Yrad);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_EllipseFilled(uint16_t  X, uint16_t  Y, uint16_t  Xrad, uint16_t  Yrad, uint16_t  colour)
{
	WriteInt(F_gfx_EllipseFilled);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Xrad);
	WriteInt(Yrad);
	WriteInt(colour);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_FrameDelay(uint16_t  Msec)
{
	WriteInt(F_gfx_FrameDelay);
	WriteInt(Msec);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_Get(uint16_t  Mode)
{
	WriteInt(F_gfx_Get);
	WriteInt(Mode);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_GetPixel(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_GetPixel);
	WriteInt(X);
	WriteInt(Y);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Line(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_Line);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_LinePattern(uint16_t  Pattern)
{
	WriteInt(F_gfx_LinePattern);
	WriteInt(Pattern);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_LineTo(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_LineTo);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_MoveTo(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_MoveTo);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_Orbit(uint16_t  Angle, uint16_t  Distance, uint16_t *  Xdest, uint16_t *  Ydest)
{
	WriteInt(F_gfx_Orbit);
	WriteInt(Angle);
	WriteInt(Distance);
	GetAck2Words(Xdest,Ydest);

	return 0 ;
}

uint16_t Pixxi_Serial_4DLib::gfx_OutlineColour(uint16_t  colour)
{
	WriteInt(F_gfx_OutlineColour);
	WriteInt(colour);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Panel(uint16_t  Raised, uint16_t  X, uint16_t  Y, uint16_t  Width, uint16_t  Height, uint16_t  colour)
{
	WriteInt(F_gfx_Panel);
	WriteInt(Raised);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Width);
	WriteInt(Height);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Polygon(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  colour)
{
	WriteInt(F_gfx_Polygon);
	WriteInt(n);
	WriteWords(Xvalues, n);
	WriteWords(Yvalues, n);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_PolygonFilled(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  colour)
{
	WriteInt(F_gfx_PolygonFilled);
	WriteInt(n);
	WriteWords(Xvalues, n);
	WriteWords(Yvalues, n);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Polyline(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  colour)
{
	WriteInt(F_gfx_Polyline);
	WriteInt(n);
	WriteWords(Xvalues, n);
	WriteWords(Yvalues, n);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_PutPixel(uint16_t  X, uint16_t  Y, uint16_t  colour)
{
	WriteInt(F_gfx_PutPixel);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Rectangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_Rectangle);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_RectangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_RectangleFilled);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}
void Pixxi_Serial_4DLib::gfx_ScreenCopyPaste(uint16_t  Xs, uint16_t  Ys, uint16_t  Xd, uint16_t  Yd, uint16_t  width, uint16_t  height)
{
	WriteInt(F_gfx_ScreenCopyPaste);
	WriteInt(Xs);
	WriteInt(Ys);
	WriteInt(Xd);
	WriteInt(Yd);
	WriteInt(width);
	WriteInt(height);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_ScreenMode(uint16_t  screenMode)
{
	WriteInt(F_gfx_ScreenMode);
	WriteInt(screenMode);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Set(uint16_t  Func, uint16_t  Value)
{
	WriteInt(F_gfx_Set);
	WriteInt(Func);
	WriteInt(Value);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_SetClipRegion()
{
	WriteInt(F_gfx_SetClipRegion);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::gfx_Slider(uint16_t  Mode, uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour, uint16_t  Scale, uint16_t  Value)
{
	WriteInt(F_gfx_Slider);
	WriteInt(Mode);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);
	WriteInt(Scale);
	WriteInt(Value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_Transparency(uint16_t  OnOff)
{
	WriteInt(F_gfx_Transparency);
	WriteInt(OnOff);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::gfx_TransparentColour(uint16_t  colour)
{
	WriteInt(F_gfx_TransparentColour);
	WriteInt(colour);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::gfx_Triangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  colour)
{
	WriteInt(F_gfx_Triangle);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(X3);
	WriteInt(Y3);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_TriangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  colour)
{
	WriteInt(F_gfx_TriangleFilled);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(X3);
	WriteInt(Y3);
	WriteInt(colour);

	GetAck();
}

void Pixxi_Serial_4DLib::gfx_Button4(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Button4);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_Switch(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Switch);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_Slider5(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Slider5);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_Dial(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Dial);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_Led(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Led);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_Gauge(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_Gauge);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_AngularMeter(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_AngularMeter);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_LedDigit(uint16_t x, uint16_t y, uint16_t digitSize, uint16_t onColour, uint16_t offColour, uint16_t value)
{
    WriteInt(F_gfx_LedDigit);
    WriteInt(x);
    WriteInt(y);
    WriteInt(digitSize);
    WriteInt(onColour);
    WriteInt(offColour);
    WriteInt(value);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_LedDigits(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_LedDigits);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

void Pixxi_Serial_4DLib::gfx_RulerGauge(uint16_t value, uint16_t hndl, uint16_t params)
{
    WriteInt(F_gfx_RulerGauge);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);

    GetAck();
}

uint16_t Pixxi_Serial_4DLib::img_ClearAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value)
{
	WriteInt(F_img_ClearAttributes);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Darken(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Darken);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Disable(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Disable);
	WriteInt(Handle);
	WriteInt(Index);

return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Enable(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Enable);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_GetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset )
{
	WriteInt(F_img_GetWord);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Offset );

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Lighten(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Lighten);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_SetAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value)
{
	WriteInt(F_img_SetAttributes);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_SetPosition(uint16_t  Handle, uint16_t  Index, uint16_t  Xpos, uint16_t  Ypos)
{
	WriteInt(F_img_SetPosition);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Xpos);
	WriteInt(Ypos);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_SetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset , uint16_t  Word)
{
	WriteInt(F_img_SetWord);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Offset);
	WriteInt(Word);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Show(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Show);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::img_Touched(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_img_Touched);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::img_FunctionCall(uint16_t imgHndl, uint16_t index, uint16_t value, uint16_t hndl, uint16_t params, uint16_t argCount, uint16_t strMap)
{
    WriteInt(F_img_FunctionCall);
    WriteInt(imgHndl);
    WriteInt(index);
    WriteInt(value);
    WriteInt(hndl);
    WriteInt(params);
    WriteInt(argCount);
    WriteInt(strMap);

    GetAck();
}

uint16_t Pixxi_Serial_4DLib::media_Flush()
{
	WriteInt(F_media_Flush);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::media_Image(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_media_Image);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::media_Init()
{
	WriteInt(F_media_Init);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::media_RdSector(uint8_t *  SectorIn)
{
	WriteInt(F_media_RdSector);

	return GetAckResSector(SectorIn);
}

uint16_t Pixxi_Serial_4DLib::media_ReadByte()
{
	WriteInt(F_media_ReadByte);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::media_ReadWord()
{
	WriteInt(F_media_ReadWord);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::media_SetAdd(uint16_t  HiWord, uint16_t  LoWord)
{
	WriteInt(F_media_SetAdd);
	WriteInt(HiWord);
	WriteInt(LoWord);

	GetAck();
}

void Pixxi_Serial_4DLib::media_SetSector(uint16_t  HiWord, uint16_t  LoWord)
{
	WriteInt(F_media_SetSector);
	WriteInt(HiWord);
	WriteInt(LoWord);

	GetAck();
}

void Pixxi_Serial_4DLib::media_Video(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_media_Video);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

void Pixxi_Serial_4DLib::media_VideoFrame(uint16_t  X, uint16_t  Y, uint16_t  Framenumber)
{
	WriteInt(F_media_VideoFrame);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Framenumber);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::media_WriteByte(uint16_t  Byte)
{
	WriteInt(F_media_WriteByte);
	WriteInt(Byte);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::media_WriteWord(uint16_t  Word)
{
	WriteInt(F_media_WriteWord);
	WriteInt(Word);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::media_WrSector(uint8_t *  SectorOut)
{
	WriteInt(F_media_WrSector);
	WriteBytes(SectorOut, 512);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::mem_Alloc(uint16_t size)
{
	WriteInt(F_mem_Alloc);
	WriteInt(size);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::mem_Free(uint16_t  Handle)
{
	WriteInt(F_mem_Free);
	WriteInt(Handle);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::mem_Heap()
{
	WriteInt(F_mem_Heap);
	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::pin_HI(uint16_t Pin)
{
	WriteInt(F_pin_HI);
	WriteInt(Pin);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::peekM(uint16_t  Address)
{
	WriteInt(F_peekM);
	WriteInt(Address);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::pin_LO(uint16_t Pin)
{
	WriteInt(F_pin_LO);
	WriteInt(Pin);

	return GetAckResp() ;
}

uint16_t Pixxi_Serial_4DLib::pin_Read(uint16_t Pin)
{
	WriteInt(F_pin_Read);
	WriteInt(Pin);

	return GetAckResp() ;
}

uint16_t Pixxi_Serial_4DLib::pin_Set(uint16_t Mode, uint16_t Pin)
{
	WriteInt(F_pin_Set);
	WriteInt(Mode);
	WriteInt(Pin);

	return GetAckResp() ;
}

void Pixxi_Serial_4DLib::putCH(uint16_t  WordChar)
{
	WriteInt(F_putCH);
	WriteInt(WordChar);

	GetAck() ;
}

void Pixxi_Serial_4DLib::pokeM(uint16_t  Address, uint16_t  WordValue)
{
	WriteInt(F_pokeM);
	WriteInt(Address);
	WriteInt(WordValue);

	GetAck() ;
}

uint16_t Pixxi_Serial_4DLib::putstr(char *  InString)
{
	WriteInt(F_putstr);
	WriteChars(InString);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::print(const char str[])
{
  int len = strlen(str);
  for(int x = 0; x<len; x++)
  {
	putCH(str[x]);
  }
}

void Pixxi_Serial_4DLib::println(const char c[])
{
  print(c);
  putCH('\n');
  putCH('\r');
}

//-Private:

void Pixxi_Serial_4DLib::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  int count = 0;
  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    buf[count] = c < 10 ? c + '0' : c + 'A' - 10;
    count++;
  } while(n);

  for(int x = count - 1; x>=0; x--)
  {
	putCH(buf[x]);
  }
}

//--------------------------------------------------------

void Pixxi_Serial_4DLib::snd_BufSize(uint16_t  Bufsize)
{
	WriteInt(F_snd_BufSize);
	WriteInt(Bufsize);

	GetAck();
}

void Pixxi_Serial_4DLib::snd_Continue()
{
	WriteInt(F_snd_Continue);

	GetAck();
}

void Pixxi_Serial_4DLib::snd_Pause()
{
	WriteInt(F_snd_Pause);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::snd_Pitch(uint16_t  Pitch)
{
	WriteInt(F_snd_Pitch);
	WriteInt(Pitch);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::snd_Playing()
{
	WriteInt(F_snd_Playing);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::snd_Stop()
{
	WriteInt(F_snd_Stop);

	GetAck();
}

void Pixxi_Serial_4DLib::snd_Volume(uint16_t  Volume)
{
	WriteInt(F_snd_Volume);
	WriteInt(Volume);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::sys_Sleep(uint16_t  Units)
{
	WriteInt(F_sys_Sleep);
	WriteInt(Units);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::touch_DetectRegion(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2)
{
	WriteInt(F_touch_DetectRegion);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::touch_Get(uint16_t  Mode)
{
	WriteInt(F_touch_Get);
	WriteInt(Mode);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::touch_Set(uint16_t  Mode)
{
	WriteInt(F_touch_Set);
	WriteInt(Mode);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::txt_Attributes(uint16_t  Attribs)
{
	WriteInt(F_txt_Attributes);
	WriteInt(Attribs);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_BGcolour(uint16_t  colour)
{
	WriteInt(F_txt_BGcolour);
	WriteInt(colour);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Bold(uint16_t  Bold)
{
	WriteInt(F_txt_Bold);
	WriteInt(Bold);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_FGcolour(uint16_t  colour)
{
	WriteInt(F_txt_FGcolour);
	WriteInt(colour);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_FontID(uint16_t  FontNumber)
{
	WriteInt(F_txt_FontID);
	WriteInt(FontNumber);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Height(uint16_t  Multiplier)
{
	WriteInt(F_txt_Height);
	WriteInt(Multiplier);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Inverse(uint16_t  Inverse)
{
	WriteInt(F_txt_Inverse);
	WriteInt(Inverse);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Italic(uint16_t  Italic)
{
	WriteInt(F_txt_Italic);
	WriteInt(Italic);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::txt_MoveCursor(uint16_t  Line, uint16_t  Column)
{
	WriteInt(F_txt_MoveCursor);
	WriteInt(Line);
	WriteInt(Column);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::txt_Opacity(uint16_t  TransparentOpaque)
{
	WriteInt(F_txt_Opacity);
	WriteInt(TransparentOpaque);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::txt_Set(uint16_t  Func, uint16_t  Value)
{
	WriteInt(F_txt_Set);
	WriteInt(Func);
	WriteInt(Value);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::txt_Underline(uint16_t  Underline)
{
	WriteInt(F_txt_Underline);
	WriteInt(Underline);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Width(uint16_t  Multiplier)
{
	WriteInt(F_txt_Width);
	WriteInt(Multiplier);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Wrap(uint16_t  Position)
{
	WriteInt(F_txt_Wrap);
	WriteInt(Position);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::txt_Xgap(uint16_t  Pixels)
{
	WriteInt(F_txt_Xgap);
	WriteInt(Pixels);

	return GetAckResp() ;
}

uint16_t Pixxi_Serial_4DLib::txt_Ygap(uint16_t  Pixels)
{
	WriteInt(F_txt_Ygap);
	WriteInt(Pixels);

	return GetAckResp() ;
}

uint16_t Pixxi_Serial_4DLib::file_CallFunction(uint16_t  Handle, uint16_t  ArgCount, t4DWordArray  Args)
{
	WriteInt(F_file_CallFunction);
	WriteInt(Handle);
	WriteInt(ArgCount);
	WriteWords(Args, ArgCount);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::sys_GetModel(char *  ModelStr)
{
	WriteInt(F_sys_GetModel);

	return GetAckResStr(ModelStr);
}

uint16_t Pixxi_Serial_4DLib::sys_GetVersion()
{
	WriteInt(F_sys_GetVersion);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::sys_GetPmmC()
{
	WriteInt(F_sys_GetPmmC);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::writeString(uint16_t  Handle, char *  StringOut)
{
	WriteInt(F_writeString);
	WriteInt(Handle);
	WriteChars(StringOut);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::readString(uint16_t  Handle, char *  StringIn)
{
	WriteInt(F_readString);
	WriteInt(Handle);

	return GetAckResStr(StringIn);
}

void Pixxi_Serial_4DLib::blitComtoDisplay(uint16_t  X, uint16_t  Y, uint16_t  Width, uint16_t  Height, uint8_t *  Pixels)
{
	WriteInt(F_blitComtoDisplay);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Width);
	WriteInt(Height);
	WriteBytes(Pixels, Width*Height*2);

	GetAck();
	}

uint16_t Pixxi_Serial_4DLib::file_FindFirstRet(char *  Filename, char *  StringIn)
{
	WriteInt(F_file_FindFirstRet);
	WriteChars(Filename);

	return GetAckResStr(StringIn);
}

uint16_t Pixxi_Serial_4DLib::file_FindNextRet(char *  StringIn)
{
	WriteInt(F_file_FindNextRet);

	return GetAckResStr(StringIn);
}

void Pixxi_Serial_4DLib::setbaudWait(uint16_t  Newrate)
{
	WriteInt(F_setbaudWait);
	WriteInt(Newrate);
	SetThisBaudrate(Newrate); // change this systems baud rate to match new display rate, ACK is 100ms away

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::widget_Create(uint16_t count)
{
	WriteInt(F_widget_Create);
	WriteInt(count);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::widget_Add(uint16_t hndl, uint16_t index, uint16_t widget)
{
	WriteInt(F_widget_Add);
	WriteInt(hndl);
	WriteInt(index);
	WriteInt(widget);

	GetAck() ;
}

void Pixxi_Serial_4DLib::widget_Delete(uint16_t hndl, uint16_t index)
{
	WriteInt(F_widget_Delete);
	WriteInt(hndl);
	WriteInt(index);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::widget_Realloc(uint16_t hndl, uint16_t count)
{
	WriteInt(F_widget_Realloc);
	WriteInt(hndl);
	WriteInt(count);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_SetWord(uint16_t Handle, uint16_t  Index, uint16_t  Offset , uint16_t  Word)
{
	WriteInt(F_widget_SetWord);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Offset);
	WriteInt(Word);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_GetWord(uint16_t  Handle, uint16_t  Index, uint16_t  Offset )
{
	WriteInt(F_widget_GetWord);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Offset);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_SetPosition(uint16_t  Handle, uint16_t  Index, uint16_t  Xpos, uint16_t  Ypos)
{
	WriteInt(F_widget_SetPosition);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Xpos);
	WriteInt(Ypos);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_Enable(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_widget_Enable);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_Disable(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_widget_Disable);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_SetAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value)
{
	WriteInt(F_widget_SetAttributes);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_ClearAttributes(uint16_t  Handle, uint16_t  Index, uint16_t  Value)
{
	WriteInt(F_widget_ClearAttributes);
	WriteInt(Handle);
	WriteInt(Index);
	WriteInt(Value);

	return GetAckResp();
}

uint16_t Pixxi_Serial_4DLib::widget_Touched(uint16_t  Handle, uint16_t  Index)
{
	WriteInt(F_widget_Touched);
	WriteInt(Handle);
	WriteInt(Index);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::widget_InitGradRAM(uint16_t hndl)
{
	WriteInt(F_widget_InitGradRAM);
	WriteInt(hndl);

	GetAck();
}

uint16_t Pixxi_Serial_4DLib::widget_InitString(char * str)
{
	uint16_t len = strlen(str);
	uint16_t addr = mem_Alloc(len);
	SendByteArrayToRAM(addr, len, (uint8_t *) str);

	return addr;
}

uint16_t Pixxi_Serial_4DLib::widget_InitStringPtr(char * str)
{
	return str_Ptr(widget_InitString(str));
}

uint16_t Pixxi_Serial_4DLib::widget_InitStringArray(char * str, uint16_t len)
{
	uint16_t addr = mem_Alloc(len);
	SendByteArrayToRAM(addr, len, (uint8_t *) str);
	addr = str_Ptr(addr);

	return addr;
}

void Pixxi_Serial_4DLib::widget_Init(uint16_t len, uint16_t * data, uint16_t * hndl, uint16_t * param)
{
	*param = mem_Alloc(len << 1);
	SendWordArrayToRAM(*param, len, data);
	*hndl = mem_Alloc(24);
}

uint16_t Pixxi_Serial_4DLib::str_Ptr(uint16_t buffer)
{
	WriteInt(F_str_Ptr);
	WriteInt(buffer);

	return GetAckResp();
}

void Pixxi_Serial_4DLib::SendWordArrayToRAM(uint16_t  hndl, uint16_t  length, uint16_t * data)
{
	WriteInt(F_sendWordArrayToRAM);
	WriteInt(hndl);
	WriteInt(length);
	WriteWords(data, length);

	GetAck();
}

void Pixxi_Serial_4DLib::SendByteArrayToRAM(uint16_t  hndl, uint16_t  length, uint8_t * data)
{
	WriteInt(F_sendByteArrayToRAM);
	WriteInt(hndl);
	WriteInt(length);
	WriteBytes(data, length);

	GetAck() ;
}
