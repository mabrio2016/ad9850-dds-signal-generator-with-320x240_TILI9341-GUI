// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       320x240_TILI9341_7Segs_GUI_Buttons.ino
    Created:	3/20/2022 9:38:27 AM
    Author:     Marco Flag
*/

#pragma region Included Libs
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include "Math.h"
#include <RotaryEncoder.h>
#pragma endregion

#pragma region Display Colors
#define BLACK			ILI9341_BLACK      	//0x0000 /*   0,   0,   0 */
#define NAVY			ILI9341_NAVY       	//0x000F /*   0,   0, 128 */
#define DARKGREEN		ILI9341_DARKGREEN  	//0x03E0 /*   0, 128,   0 */
#define DARKCYAN		ILI9341_DARKCYAN   	//0x03EF /*   0, 128, 128 */
#define MAROON			ILI9341_MAROON     	//0x7800 /*   128, 0,   0 */
#define PURPLE			ILI9341_PURPLE     	//0x780F /*   128,  0, 128 */
#define OLIVE			ILI9341_OLIVE      	//0x7BE0 /*   128, 128,  0 */
#define LIGHTGREY		ILI9341_LIGHTGREY  	//0xC618 /*   192, 192, 192 */
#define DARKGREY		ILI9341_DARKGREY   	//0x7BEF /*   128, 128, 128 */
#define BLUE			ILI9341_BLUE       	//0x001F /*   0,     0, 255 */
#define GREEN			ILI9341_GREEN      	//0x07E0 /*   0,   255,   0 */
#define CYAN			ILI9341_CYAN       	//0x07FF /*   0,   255, 255 */
#define RED				ILI9341_RED        	//0xF800 /*   255,   0,   0 */
#define MAGENTA			ILI9341_MAGENTA    	//0xF81F /*   255,   0, 255 */
#define YELLOW			ILI9341_YELLOW     	//0xFFE0 /*   255, 255,   0 */
#define WHITE			ILI9341_WHITE      	//0xFFFF /*   255, 255, 255 */
#define ORANGE			ILI9341_ORANGE     	//0xFD20 /*   255, 165,   0 */
#define GREENYELLOW		ILI9341_GREENYELLOW //0xAFE5 /*   173, 255,  47 */
#define PINK			ILI9341_PINK       	//0xF81F
#pragma endregion

#pragma region variable definitions
boolean rst_clickButton = false;
boolean mlt_clickButton = false;
boolean Rotary_enabled = 1;
uint32_t Number = 0;
int Steep = 1;
int AutoCountFlag = 0;
int Multiply = 0;
#pragma endregion

#pragma region Rotary Encoder definitions
#define PIN_IN1 2 // interrupt enabled 
#define PIN_IN2 3 // interrupt enabled
static int pos = 0;
static int newPos = 0;
static int Direction = 0;
#pragma endregion

#pragma region TFT SPI display - SPI Display/TouchScreen definitions
#define TFT_CS 10
#define TFT_DC 9
#define TFT_MOSI 11 //Also connected Touchscreen "T_DIN" pin
#define TFT_CLK 13  //Also connected Touchscreen "T_CLK" pin
#define TFT_RST 8
#define TFT_MISO 12 //Also connected Touchscreen "T_DO" pin


#define T_CS 7   //Touchscreen T_CS pin (Chip select)
#define T_IRQ 2  //Touchscreen T_IRQ pin (Chip interrupt request)
#pragma endregion

#pragma region Disply/Touch definitions and initialization
#define MINPRESSURE 150
#define MAXPRESSURE 1000
#define ROTATION 1
#define displyBackground BLACK
//Calibration values // These values were captured after Touchscreen calibration.
float xCalM = -0.09, yCalM = 0.07; // gradients
float xCalC = 335.01, yCalC = -15.18; // y axis crossing points
TS_Point p; // This Class is defined in the XPT2046 library for Touch Controller
    //Initiate the TFT disply
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
// Initiate the TouchScreen.
XPT2046_Touchscreen ts(T_CS);
#pragma endregion

#pragma region GUI Objects definitions
//7 Sements rendering position//
#define SEG_FRAME_X 20
#define SEG_FRAME_Y 30 
    //Reset button parameters//
#define rst_BUTTON_X 285
#define rst_BUTTON_Y 220
#define rst_BUTTON_W 70
#define rst_BUTTON_H 40
    //Multyply button parameters//
#define mlt_BUTTON_X 285
#define mlt_BUTTON_Y 175
#define mlt_BUTTON_W 70
#define mlt_BUTTON_H 40
#pragma endregion

#pragma region AD9850 definitions
#define DATA 14 // Pin 4 - data Line of the serial interface (DATE) 
#define W_CLK 15 // pin 2 - Signal, the timer bus (W_CLK) 
#define FQ_UD 16 // Pin 3 - change the frequency (FQ) 
#define RESET 17 // Pin 5 - Reset module (RESET)
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); } // a function of the heartbeat is set at the time the high status 
#pragma endregion

#pragma region Classes definition
class ScreenPoint {
public:
    int16_t x;
    int16_t y;

    ScreenPoint() {
    } //Default contructor

    ScreenPoint(int16_t xIn, int16_t yIn) {
        x = xIn;
        y = yIn;
    }
};
class SevenSegDisplay {
private:
    const int digit_array[10][7] = {
    {1, 1, 1, 1, 1, 1, 0},  // 0
    {0, 1, 1, 0, 0, 0, 0},  // 1
    {1, 1, 0, 1, 1, 0, 1},  // 2
    {1, 1, 1, 1, 0, 0, 1},  // 3
    {0, 1, 1, 0, 0, 1, 1},  // 4
    {1, 0, 1, 1, 0, 1, 1},  // 5
    {1, 0, 1, 1, 1, 1, 1},  // 6
    {1, 1, 1, 0, 0, 0, 0},  // 7
    {1, 1, 1, 1, 1, 1, 1},  // 8
    {1, 1, 1, 1, 0, 1, 1}   // 9
    };
    int previous_digit[6] = { 0,0,0,0,0,0 }; // Used to confirm if new digit is difernt compared with the old one.

    uint32_t digit;
    uint16_t color;
    int size = 2;
    uint8_t spacing_x = 30;

protected:
    int pos_x;
    int pos_y;

    //void render_digit(uint16_t pos_x, uint16_t pos_y, uint32_t digit, uint16_t color) {  //Small size not implemented yet
    //    // loop through 7 segments
    //    for (uint8_t i = 0; i < 7; i++) {
    //        bool seg_on = digit_array[digit][i];
    //        // if seg_on is true draw segment
    //        if (seg_on) {
    //            switch (i) {
    //            case 0:
    //                tft.fillRoundRect(2 + pos_x, 0 + pos_y, 9, 3, 2, color); // SEG a
    //                break;
    //            case 1:
    //                tft.fillRoundRect(10 + pos_x, 2 + pos_y, 3, 9, 2, color); // SEG b
    //                break;
    //            case 2:
    //                tft.fillRoundRect(10 + pos_x, 12 + pos_y, 3, 9, 2, color); // SEG c
    //                break;
    //            case 3:
    //                tft.fillRoundRect(2 + pos_x, 20 + pos_y, 9, 3, 2, color); // SEG d
    //                break;
    //            case 4:
    //                tft.fillRoundRect(0 + pos_x, 12 + pos_y, 3, 9, 2, color); // SEG e
    //                break;
    //            case 5:
    //                tft.fillRoundRect(0 + pos_x, 2 + pos_y, 3, 9, 2, color); // SEG f
    //                break;
    //            case 6:
    //                tft.fillRoundRect(2 + pos_x, 10 + pos_y, 9, 3, 2, color); // SEG g
    //                break;
    //            }
    //            seg_on = false;
    //        }
    //    }
    //}

public:
    //Contructor
    SevenSegDisplay(int pos_x_, int pos_y_, uint16_t color) {
        this->pos_x = pos_x_;
        this->pos_y = pos_y_;
        this->digit = digit;
        this->color = color;
    };
    void render_digit_segments_big(int pos_x, int pos_y, uint16_t digit, uint16_t color) {
        // loop through 7 segments
        for (uint8_t i = 0; i < 7; i++) {
            bool seg_on = digit_array[digit][i];
            // if seg_on is true draw segment
            if (seg_on) {
                switch (i) {
                case 0:
                    tft.fillRoundRect(4 + pos_x, 0 + pos_y, 18, 6, 4, color); // SEG a;
                    break;
                case 1:
                    tft.fillRoundRect(20 + pos_x, 4 + pos_y, 6, 20, 4, color); // SEG b
                    break;
                case 2:
                    tft.fillRoundRect(20 + pos_x, 26 + pos_y, 6, 20, 4, color); // SEG c
                    break;
                case 3:
                    tft.fillRoundRect(4 + pos_x, 44 + pos_y, 18, 6, 4, color); // SEG d
                    break;
                case 4:
                    tft.fillRoundRect(0 + pos_x, 26 + pos_y, 6, 20, 4, color); // SEG e
                    break;
                case 5:
                    tft.fillRoundRect(0 + pos_x, 4 + pos_y, 6, 20, 4, color); // SEG f
                    break;
                case 6:
                    tft.fillRoundRect(4 + pos_x, 22 + pos_y, 18, 6, 4, color); // SEG g
                    break;
                }
                seg_on = false;
            }
        }
    }
    void render_digit_segments(uint32_t digit) {
        uint8_t digit_array[] = { digit / 100000 % 10, digit / 10000 % 10, digit / 1000 % 10, digit / 100 % 10, digit / 10 % 10, digit % 10 };
        if (digit > 99999) {
            if (previous_digit[6] != digit_array[0])
            {
                tft.fillRect(pos_x, pos_y - 5, 30, 60, WHITE);
                render_digit_segments_big(pos_x, pos_y, digit_array[0], color);
                previous_digit[6] = digit_array[0];
            }
        }
        else {
            tft.fillRect(pos_x, pos_y - 5, 30, 60, WHITE);
        }
        if (digit > 9999) {
            if (previous_digit[5] != digit_array[1]) {
                tft.fillRect(pos_x + spacing_x, pos_y - 5, 45, 60, WHITE);
                render_digit_segments_big(pos_x + spacing_x, pos_y, digit_array[1], color);
                previous_digit[5] = digit_array[1];
            }
        }
        else {
            tft.fillRect(pos_x + spacing_x, pos_y - 5, 30, 60, WHITE);
        }
        if (digit > 999) {
            if (previous_digit[4] != digit_array[2]) {
                tft.fillRect(pos_x + (spacing_x * 2), pos_y - 5, 45, 60, WHITE);
                render_digit_segments_big(pos_x + (spacing_x * 2), pos_y, digit_array[2], color);
                previous_digit[4] = digit_array[2];
                tft.fillRoundRect(32 + (pos_x + (spacing_x * 2)), 42 + pos_y, 8, 8, 4, RED); // Decimal point
            }
        }
        else {
            tft.fillRect(pos_x + (spacing_x * 2), pos_y - 5, 30, 60, WHITE);
            tft.fillRect(30 + pos_x + (spacing_x * 2), pos_y - 5, 15, 60, WHITE); // Used to fill the Decimal point space.
        }
        if (digit > 99) {
            if (previous_digit[3] != digit_array[3]) {
                tft.fillRect(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
                render_digit_segments_big(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y, digit_array[3], color);
                previous_digit[3] = digit_array[3];
            }
        }
        else {
            tft.fillRect(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
        }
        if (digit > 9) {
            if (previous_digit[2] != digit_array[4]) {
                tft.fillRect(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
                render_digit_segments_big(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y, digit_array[4], color);
                previous_digit[2] = digit_array[4];
            }
        }
        else {
            tft.fillRect(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
        }
        if (previous_digit[1] != digit_array[5]) {
            tft.fillRect(pos_x + (spacing_x * 5 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
            render_digit_segments_big(pos_x + (spacing_x * 5 + (spacing_x / 2)), pos_y, digit_array[5], color);
            previous_digit[1] = digit_array[5];  // previous_Number must be an array with digits 
        }
    }
    void drawFrame(uint16_t frameColor) //7 segment frame//
    {
        tft.fillRect(SEG_FRAME_X - 4, SEG_FRAME_Y - 9, 15 + 8 + 30 * 6, 60 + 8, WHITE);
        tft.drawRect(SEG_FRAME_X - 4, SEG_FRAME_Y - 9, 15 + 8 + 30 * 6, 60 + 8, frameColor); // (x, y, w, h)
        tft.drawRect(SEG_FRAME_X - 2, SEG_FRAME_Y - 7, 15 + 4 + 30 * 6, 60 + 4, frameColor);
    }
};
class up_downButton {
private:

    int X_arrwsPodition;
    int Y_arrwsPodition;
    int arrwWidth;
    int arrwHeight;
    uint16_t frameColor;
    uint16_t errowColor;

    typedef struct {
        int X;
        int Y;
        int width;
        int height;
    }rect;
    typedef struct {
        int Min;
        int Max;
        int Init;
        int Step;
    }arrowRec;

    rect arrwFrameR;
    rect upArrwR;
    rect dwnArrwR;
    TS_Point upArrwTip, upArrwRBase, upArrwLBase;
    TS_Point dwnArrwTip, dwnArrwRBase, dwnArrwLBase;

public:
    //Constructor
    up_downButton(int X_arrwsPodition, int Y_arrwsPodition, int arrwWidth, int arrwHeight, uint16_t frameColor, uint16_t errowColor) {
        this->X_arrwsPodition = X_arrwsPodition;
        this->Y_arrwsPodition = Y_arrwsPodition;
        this->arrwWidth = arrwWidth;
        this->arrwHeight = arrwHeight;
        this->frameColor = frameColor;
        this->errowColor = errowColor;

        arrwFrameR = { X_arrwsPodition, Y_arrwsPodition, arrwWidth, arrwHeight * 2 };
        upArrwR = { arrwFrameR.X, arrwFrameR.Y, arrwWidth, arrwHeight };
        dwnArrwR = { arrwFrameR.X, arrwFrameR.Y + arrwFrameR.height / 2, arrwWidth, arrwHeight };
    };
    void littleArrows()
    {
        tft.drawRect(arrwFrameR.X + 1, arrwFrameR.Y + 1, arrwFrameR.width, arrwFrameR.height, frameColor);
        //tft.fillRect(arrwFrameR.X + 2, arrwFrameR.Y + 2, arrwFrameR.width - 2, arrwFrameR.height - 2, YELLOW);
        upArrwTip.x = upArrwR.X + upArrwR.width / 2;
        upArrwTip.y = upArrwR.Y + 5;
        upArrwLBase.x = upArrwR.X + 5;
        upArrwLBase.y = upArrwR.Y + upArrwR.height - 5;
        upArrwRBase.x = upArrwR.X + upArrwR.width - 5;
        upArrwRBase.y = upArrwR.Y + upArrwR.height - 5;
        tft.fillTriangle(upArrwTip.x, upArrwTip.y, upArrwLBase.x, upArrwLBase.y, upArrwRBase.x, upArrwRBase.y, errowColor);
        dwnArrwTip.x = dwnArrwR.X + dwnArrwR.width / 2;
        dwnArrwTip.y = dwnArrwR.Y + dwnArrwR.height - 5;
        dwnArrwLBase.x = dwnArrwR.X + 5;
        dwnArrwLBase.y = dwnArrwR.Y + 5;
        dwnArrwRBase.x = dwnArrwR.X + dwnArrwR.width - 5;
        dwnArrwRBase.y = dwnArrwR.Y + 5;
        tft.fillTriangle(dwnArrwTip.x, dwnArrwTip.y, dwnArrwLBase.x, dwnArrwLBase.y, dwnArrwRBase.x, dwnArrwRBase.y, errowColor);
    }

    int isErrowPressed(ScreenPoint sp_) {
        if ((sp_.x > upArrwR.X - upArrwR.width / 2) && (sp_.x < (upArrwR.X + (upArrwR.width / 2.5)))) {
            if ((sp_.y > upArrwR.Y) && (sp_.y <= (upArrwR.Y + upArrwR.height))) {
                //Serial.println("up Errow btn hit");
                //upArrowHandler();
                return 1;
            }
        }
        if ((sp_.x > dwnArrwR.X - upArrwR.width / 2) && (sp_.x < (dwnArrwR.X + dwnArrwR.width / 2.5))) {
            if ((sp_.y > dwnArrwR.Y) && (sp_.y <= (dwnArrwR.Y + dwnArrwR.height))) {
                //Serial.println("down Errow btn hit");
                //downArrowHandler();
                return 2;
            }
        }
    }
};
class CheckBox {
private:
    int xPos_;
    int yPos_;
    int boxWidth_;
    int boxHeight_;
    char* rectText_;
    uint16_t checked_BOX_color_;   // = ILI9341_GREEN;
    uint16_t unchecked_BOX_color_; // = displyBackground;
    uint16_t rect_color_;   // Color of the full Box
public:
    boolean chkBoxFlag = false;
    //Contructor
    CheckBox(int xPos_, int yPos_, int boxWidth_, int boxHeight, char* rectText_, uint16_t rect_color_) {
        this->xPos_ = xPos_;
        this->yPos_ = yPos_;
        this->boxWidth_ = boxWidth_;
        this->boxHeight_ = boxWidth_;
        this->rectText_ = rectText_;
        this->checked_BOX_color_ = checked_BOX_color_;
        this->unchecked_BOX_color_ = unchecked_BOX_color_;
        this->rect_color_ = rect_color_;
    };
    void drawFrame() {
        tft.fillRect(xPos_ - 5, yPos_ - 5, boxWidth_ + 60, boxHeight_ + 10, rect_color_);
        uint16_t check_BOX_color_Frame = checked_BOX_color_ >> 6;
        tft.drawRect(xPos_ - 2, yPos_ - 2, boxWidth_ + 4, boxHeight_ + 4, check_BOX_color_Frame); // (x, y, w, h)
        tft.fillRect(xPos_, yPos_, boxWidth_, boxHeight_, unchecked_BOX_color_);
        uint16_t rect_BOX_color_Frame = checked_BOX_color_ << 3;
        tft.drawRect(xPos_ - 5, yPos_ - 5, boxWidth_ + 60, boxHeight_ + 11, rect_BOX_color_Frame);

        tft.setCursor(xPos_ + 24, yPos_ + 15);
        tft.setTextSize(1);
        tft.setTextColor(RED);
        tft.print(rectText_);
    }
    int IsChecked(ScreenPoint sp_) {
        if ((sp_.x > xPos_ - boxWidth_) && (sp_.x < (xPos_ + (boxWidth_ * 2.5)))) {
            if ((sp_.y > yPos_ + 10) && (sp_.y < (yPos_ + boxHeight_ + 25))) {
                delay(200);
                if (chkBoxFlag == false) {
                    chkBoxFlag = true;
                    //return 1;
                }
                else {
                    chkBoxFlag = false;
                    checked(displyBackground);
                    //return 0;
                }
            }
        }
        return chkBoxFlag;
    }
    void checked(uint16_t chk_color) {
        tft.fillRect(xPos_, yPos_, boxWidth_, boxHeight_, chk_color);
    }
};
#pragma endregion

#pragma region Classe instances
SevenSegDisplay SevenSeg1 = SevenSegDisplay(SEG_FRAME_X, SEG_FRAME_Y, BLACK);
up_downButton up_downButton_1 = up_downButton(223, 22, 32, 32, WHITE, BLUE);
CheckBox CheckBox_1 = CheckBox(5, 215, 20, 20, "Auto", YELLOW);
CheckBox CheckBox_2 = CheckBox(5, 175, 20, 20, "X100", GREEN);
Adafruit_GFX_Button Reset_BTN;
Adafruit_GFX_Button Multiply_BTN;
RotaryEncoder* encoder = nullptr; //A pointer to the dynamic created rotary encoder instance. // This will be done in setup()
#pragma endregion

#pragma region Functions definition
ScreenPoint getScreenCoords(int16_t x, int16_t y) {
    int16_t xCoord = round((x * xCalM) + xCalC);
    int16_t yCoord = round((y * yCalM) + yCalC);
    if (xCoord < 0) xCoord = 0;
    if (xCoord >= tft.width()) xCoord = tft.width() - 1;
    if (yCoord < 0) yCoord = 0;
    if (yCoord >= tft.height()) yCoord = tft.height() - 1;
    return(ScreenPoint(xCoord, yCoord));
}
int buttonPressed_Handler(int xPos_, int yPos_, int buttonWidth_, int buttonHeight_, ScreenPoint sp_, Adafruit_GFX_Button* btn_Name) {
    if ((sp_.x > (xPos_ - (buttonWidth_ / 1.2))) && (sp_.x < (xPos_ + (buttonWidth_ / 3.5)))) {
        if ((sp_.y > yPos_ - (buttonHeight_ / 4)) && (sp_.y <= (yPos_ + (buttonHeight_ / 1.5)))) {
            //Serial.println(btn_Name);
            btn_Name->drawButton(true);
            Steep = 1;
            return 1;
        }
    }
    else return 0;
}
void Autocount() {
    if (Multiply == 1) {
        Number = (Number + Steep) + round(Number / 100) * Steep;
    }
    else if (Multiply == 0) {
        Number = Number + Steep;
    }
    check_Number();
}
void Rotary_checkPosition() //Rotary encoder
{
    encoder->tick(); // just call tick() to check the state.
    if (Rotary_enabled == true) {
        newPos = encoder->getPosition();
        if (pos != newPos) {
            //Serial.print("pos:");
            //Serial.print(newPos);
            //Serial.print(" dir:");
            //Serial.println((int)(encoder->getDirection()));
            //Steep = 1;        
            Direction = (int)encoder->getDirection();
            if (Direction == -1) {
                if (Multiply == 1) {
                    Number = (Number + Steep) + round(Number / 100) * Steep; ;
                }
                else if (Multiply == 0) {
                    Number = Number + Steep;
                }
            }
            else if (Direction == 1) {
                if (Multiply == 1) {
                    Number = (Number - 1) - round(Number / 100 * Steep);
                }
                else if (Multiply == 0) {
                    Number = Number - Steep;
                }
            };
        };
        if (Number > 9999999) { //Keep the Number within the range 0 to 999999 - when decrementing counting number was changing to 4294967295 - This helps to workaroud the bug.
            Number = 0;
        }
        if (Number >= 1000000 && Number <= 9999999) { //Keep the Number within the range 0 to 999999
            Number = 999999;
        }
        /*Serial.print(" Number:");
        Serial.println(Number);*/
    };
}
void tfr_byte(byte data) // transfer byte bit after bit to the AD9850 module via the DATA line 
{
    for (int i = 0; i < 8; i++, data >>= 1) {
        digitalWrite(DATA, data & 0x01);
        pulseHigh(W_CLK); // after each bit signal CLK is set to high 
    }
}
void set_Frequency(double frequency) { // frequency is calculated according to the formula on page 8 documentation = */2^32 
    int32_t freq = frequency * 4294967295 / 125000000; // frequency 125 MHz AD9850 
    for (int b = 0; b < 4; b++, freq >>= 8) {
        tfr_byte(freq & 0xFF);
    }
    tfr_byte(0x000); // control byte of zero "0" for module AD9850 
    pulseHigh(FQ_UD); // update frequency 
}
int check_Number() /*Keep number limit between 0 and 999999*/ {
    if (Number >= 0 && Number <= 999999) {
        Steep = 1;
        RenderSend_Nunber();
        Rotary_enabled = true;
        return 1;
    }
    if (Number < 0) {
        Number = 0;
        Steep = 0;
        pos = 0;
        newPos = 0;
        Rotary_enabled = false;
        return -1;
    }
    else if (Number > 999999) {
        Number = 999999;
        Steep = 0;
        pos = 0;
        newPos = 0;
        Rotary_enabled = false;
        return -2;
    }
}
void RenderSend_Nunber() {
    SevenSeg1.render_digit_segments(Number);
    set_Frequency(Number); // Set the frequency for the frequency generator
    if (Number <= 0) {
        Number = 1; // Need a sart Number.
    }
}
#pragma endregion

void setup()
{
    Serial.begin(9600);
    tft.begin();
    tft.setRotation(ROTATION);
    tft.fillScreen(displyBackground);
    ts.begin();
    ts.setRotation(ROTATION);
    tft.setFont(&FreeSerifBold9pt7b); // Bigger Fonts for the CheckBox test
    CheckBox_1.drawFrame();
    CheckBox_2.drawFrame();
    tft.setFont();
    SevenSeg1.drawFrame(RED); // Smaller fonts for the buttons text
    up_downButton_1.littleArrows();
    Reset_BTN.initButton(&tft, rst_BUTTON_X, rst_BUTTON_Y, rst_BUTTON_W, rst_BUTTON_H, GREEN, RED, YELLOW, "Reset", 2, 2);
    Reset_BTN.drawButton();
    Multiply_BTN.initButton(&tft, mlt_BUTTON_X, mlt_BUTTON_Y, mlt_BUTTON_W, mlt_BUTTON_H, CYAN, BLUE, YELLOW, "Multi", 2, 2);
    Multiply_BTN.drawButton();
    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03); // Rotary encode configurations
    // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
    attachInterrupt(digitalPinToInterrupt(PIN_IN1), Rotary_checkPosition /*function call*/, CHANGE); // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), Rotary_checkPosition /*function call*/, CHANGE); // register interrupt routine
    // Arduino - Arduino pin configuration 
    pinMode(FQ_UD, OUTPUT);
    pinMode(W_CLK, OUTPUT);
    pinMode(DATA, OUTPUT);
    pinMode(RESET, OUTPUT);
    pulseHigh(RESET);
    pulseHigh(W_CLK);
    pulseHigh(FQ_UD); // toggles serial interface 
}

void loop()
{
    if (AutoCountFlag == 1) {
        Autocount();
    }
    if (pos != newPos) /*Check Rotary encoder position*/ {
        check_Number();
        //RenderSend_Nunber();
        pos = newPos;
    }
    ScreenPoint sp;
    if (ts.touched()) /*Check wich screen object was touched */ {
        p = ts.getPoint();
        sp = getScreenCoords(p.x, p.y);
        if (CheckBox_1.IsChecked(sp) == true) {
            CheckBox_1.checked(RED);
            AutoCountFlag = 1;
        }
        else if (CheckBox_1.chkBoxFlag == false) {
            AutoCountFlag = 0;
        }
        if (CheckBox_2.IsChecked(sp) == true) {
            CheckBox_2.checked(WHITE);
            Multiply = 1;
        }
        else if (CheckBox_2.chkBoxFlag == false) {
            Multiply = 0;
        }
        if (buttonPressed_Handler(rst_BUTTON_X, rst_BUTTON_Y, rst_BUTTON_W, rst_BUTTON_H, sp, &Reset_BTN) == 1) {
            rst_clickButton = true;
            Number = 0; //0000000;
            RenderSend_Nunber();
        }
        if (buttonPressed_Handler(mlt_BUTTON_X, mlt_BUTTON_Y, mlt_BUTTON_W, mlt_BUTTON_H, sp, &Multiply_BTN) == 1) {
            mlt_clickButton = true;
            Number = Number * 1.2;
            //RenderSend_Nunber();
            check_Number();
        }
        //UP Button
        if (up_downButton_1.isErrowPressed(sp) == 1) {

            if (check_Number() == 1) {
                Number = Number + Steep;
                check_Number();
            }
        }
        //Down Button
        if (up_downButton_1.isErrowPressed(sp) == 2) {
            if (check_Number() == 1) {
                Number = Number - Steep;
                check_Number();
            }
        }
    }

    //If nothing is touched, make sure that Reset button is unclick (return to it's original color and state)
    else if (rst_clickButton == true) {
        Reset_BTN.drawButton();
        rst_clickButton = false;
    }
    //If nothing is touched, make sure that Multiply button is unclick (return to it's original color and state)
    else if (mlt_clickButton == true) {
        Multiply_BTN.drawButton();
        mlt_clickButton = false;
    }
}