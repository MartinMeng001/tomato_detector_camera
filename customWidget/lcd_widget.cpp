#include "lcd_widget.h"
#include <QDebug>
#include <QPainter>

LCDWidget::LCDWidget(QWidget *parent) : QWidget(parent),
    display(nullptr), display_char_buffer(nullptr)
{
    // Default Parameter
    column = 10;
    row = 1;

    color_background_2 = QColor(68, 68, 68);
    color_background_1 = QColor(68, 68, 68);
    color_pixel = QColor(62, 183, 27);

    SetCursor(0, 0);

    CalculateDisplaySize();
    CopyCharRomToRam();

    RefreshDisplay();
}

LCDWidget::~LCDWidget()
{
    delete display;
    if (display_char_buffer != nullptr)
        delete [] display_char_buffer;
}

void LCDWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter p(this);

    p.setRenderHints(QPainter::Antialiasing |
                     QPainter::SmoothPixmapTransform |
                     QPainter::TextAntialiasing, true);

    p.scale((float)this->width() / this->display_size_w, (float)this->height() / this->display_size_h);
    p.drawImage(0, 0, *display);
}

void LCDWidget::RefreshDisplay()
{
    display->fill(color_background_1);

    int i = 0;
    for (int y = 0; y < row; y++)
        for (int x = 0; x < column; x++) {
            DrawChar((x * (LCD_CHAR_PIXEL_SIZE_W + LCD_CHAR_SPACE_X)) + LCD_BORDER_SIZE, (y * (LCD_CHAR_PIXEL_SIZE_H + LCD_CHAR_SPACE_Y)) + LCD_BORDER_SIZE, display_char_buffer[i++]);
        }

    this->update();
}

int LCDWidget::GetCurrentColumn()
{
    return column;
}

int LCDWidget::GetCurrentRow()
{
    return row;
}

void LCDWidget::SetColumn(int column)
{
    this->column = column;
    CalculateDisplaySize();
    RefreshDisplay();
}

void LCDWidget::SetRow(int row)
{
    this->row = row;
    CalculateDisplaySize();
    RefreshDisplay();
}

void LCDWidget::SetColorBackground1(const QColor color)
{
    color_background_1 = color;
    RefreshDisplay();
}

void LCDWidget::SetColorBackground2(const QColor color)
{
    color_background_2 = color;
    RefreshDisplay();
}

void LCDWidget::SetColorPixel(const QColor color)
{
    color_pixel = color;
    RefreshDisplay();
}

QColor LCDWidget::GetColorBackground1()
{
    return color_background_1;
}

QColor LCDWidget::GetColorBackground2()
{
    return color_background_2;
}

QColor LCDWidget::GetColorPixel()
{
    return color_pixel;
}

uint8_t *LCDWidget::GetDisplayCharBuffer()
{
    return display_char_buffer;
}

int LCDWidget::GetDisplayCharBufferLength()
{
    return row * column;
}

void LCDWidget::Clear()
{
    for (int i = 0; i < row * column; i++)
        display_char_buffer[i] = ' ';
    Home();
    RefreshDisplay();
    update();
}

void LCDWidget::Home()
{
    cursor_pos_x = 0;
    cursor_pos_y = 0;
}

void LCDWidget::SetCursor(uint8_t column, uint8_t row)
{
    if (row == 0)
        return;

    cursor_pos_x = column;
    cursor_pos_y = row - 1;
}

void LCDWidget::Data(uint8_t data)
{
    int idx = cursor_pos_y * column + cursor_pos_x;
    display_char_buffer[idx] = data;

    RefreshDisplay();
}

void LCDWidget::String(QString text)
{
    for (int i = 0; i < text.length(); i++) {
        int idx = cursor_pos_y * column + cursor_pos_x;
        QChar c = text.at(i);
        display_char_buffer[idx] = (uint8_t)c.toLatin1();

        cursor_pos_x++;
        if (cursor_pos_x == column) {
            cursor_pos_x = 0;
            cursor_pos_y++;
            if (cursor_pos_y == row)
                cursor_pos_y = 0;
        }
    }
    RefreshDisplay();
}

void LCDWidget::SetUserChar(uint8_t char_nr, uint8_t *pixel_buffer)
{
    char_nr &= 0x0f;
    for (int i = 0; i < LCD_CHAR_W; i++) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; j++) {
            byte |= ((pixel_buffer[j] >> i) & 1) << (7 - j);
        }
        char_ram[char_nr][4 - i] = byte;
    }
}

bool LCDWidget::SaveImage(QString filename)
{
    return display->save(filename);
}

void LCDWidget::CalculateDisplaySize()
{
    display_size_w = 2 * LCD_BORDER_SIZE + (column - 1) * LCD_CHAR_SPACE_X + column * LCD_CHAR_PIXEL_SIZE_W;
    display_size_h = 2 * LCD_BORDER_SIZE + (row - 1) * LCD_CHAR_SPACE_Y + row * LCD_CHAR_PIXEL_SIZE_H;

    if (display_char_buffer != nullptr)
        delete [] display_char_buffer;
    display_char_buffer = new uint8_t[column * row];

    for (int i = 0; i < (column * row); i++)
        display_char_buffer[i] = ' ';

    if (display != nullptr)
        delete display;

    display = new QImage(display_size_w, display_size_h, QImage::Format_RGB32);
}

void LCDWidget::DrawChar(int x, int y, uint8_t c)
{
    for (int c_pos = 0; c_pos < 5; c_pos ++) {
        int y2 = y;
        for (int y1 = 0; y1 < LCD_CHAR_H; y1++) {
            QColor col;
            if ((char_ram[c][c_pos] >> (LCD_CHAR_H - y1 - 1)) & 1)
                col = color_pixel;
            else
                col = color_background_2;

            for (int z = 0; z < LCD_PIXEL_SIZE_H; z++) {
                for (int i = 0; i < LCD_PIXEL_SIZE_W; i++)
                    display->setPixel(x + i + c_pos * (LCD_PIXEL_SIZE_W + LCD_PIXEL_SPACE_X), y2, col.rgb());
                y2++;
            }

            for (int y1 = 0; y1 < LCD_PIXEL_SPACE_Y; y1++) {
                for (int i = 0; i < LCD_PIXEL_SIZE_W; i++)
                    display->setPixel(x + i + c_pos * (LCD_PIXEL_SIZE_W + LCD_PIXEL_SPACE_X), y2, color_background_1.rgb());
                y2++;
            }
        }
    }
}

void LCDWidget::CopyCharRomToRam()
{
    for (int i = 0; i < ROM_FONT_CHARS; i++)
        for (int j = 0; j < LCD_CHAR_W; j++)
            char_ram[i + CGRAM_STORAGE_CHARS][j] = fontA00[i][j];
}

// A00 (Japanese) character set.
// skip first 16 characters reserved for CGRAM
const uint8_t LCDWidget::fontA00[ROM_FONT_CHARS][LCD_CHAR_W] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  16 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  17 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  18 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  19 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  20 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  21 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  22 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  23 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  24 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  25 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  26 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  27 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  28 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  29 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  30 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  31 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  32 -
    {0x00, 0x00, 0xf2, 0x00, 0x00}, //  33 - !
    {0x00, 0xe0, 0x00, 0xe0, 0x00}, //  34 - "
    {0x28, 0xfe, 0x28, 0xfe, 0x28}, //  35 - #
    {0x24, 0x54, 0xfe, 0x54, 0x48}, //  36 - $
    {0xc4, 0xc8, 0x10, 0x26, 0x46}, //  37 - %
    {0x6c, 0x92, 0xaa, 0x44, 0x0a}, //  38 - &
    {0x00, 0xa0, 0xc0, 0x00, 0x00}, //  39 - '
    {0x00, 0x38, 0x44, 0x82, 0x00}, //  40 - (
    {0x00, 0x82, 0x44, 0x38, 0x00}, //  41 - )
    {0x28, 0x10, 0x7c, 0x10, 0x28}, //  42 - *
    {0x10, 0x10, 0x7c, 0x10, 0x10}, //  43 - +
    {0x00, 0x0a, 0x0c, 0x00, 0x00}, //  44 - ,
    {0x10, 0x10, 0x10, 0x10, 0x10}, //  45 - -
    {0x00, 0x06, 0x06, 0x00, 0x00}, //  46 - .
    {0x04, 0x08, 0x10, 0x20, 0x40}, //  47 - /
    {0x7c, 0x8a, 0x92, 0xa2, 0x7c}, //  48 - 0
    {0x00, 0x42, 0xfe, 0x02, 0x00}, //  49 - 1
    {0x42, 0x86, 0x8a, 0x92, 0x62}, //  50 - 2
    {0x84, 0x82, 0xa2, 0xd2, 0x8c}, //  51 - 3
    {0x18, 0x28, 0x48, 0xfe, 0x08}, //  52 - 4
    {0xe4, 0xa2, 0xa2, 0xa2, 0x9c}, //  53 - 5
    {0x3c, 0x52, 0x92, 0x92, 0x0c}, //  54 - 6
    {0x80, 0x8e, 0x90, 0xa0, 0xc0}, //  55 - 7
    {0x6c, 0x92, 0x92, 0x92, 0x6c}, //  56 - 8
    {0x60, 0x92, 0x92, 0x94, 0x78}, //  57 - 9
    {0x00, 0x6c, 0x6c, 0x00, 0x00}, //  58 - :
    {0x00, 0x6a, 0x6c, 0x00, 0x00}, //  59 - ;
    {0x10, 0x28, 0x44, 0x82, 0x00}, //  60 - <
    {0x28, 0x28, 0x28, 0x28, 0x28}, //  61 - =
    {0x00, 0x82, 0x44, 0x28, 0x10}, //  62 - >
    {0x40, 0x80, 0x8a, 0x90, 0x60}, //  63 - ?
    {0x4c, 0x92, 0x9e, 0x82, 0x7c}, //  64 - @
    {0x7e, 0x90, 0x90, 0x90, 0x7e}, //  65 - A
    {0xfe, 0x92, 0x92, 0x92, 0x6c}, //  66 - B
    {0x7c, 0x82, 0x82, 0x82, 0x44}, //  67 - C
    {0xfe, 0x82, 0x82, 0x44, 0x38}, //  68 - D
    {0xfe, 0x92, 0x92, 0x92, 0x82}, //  69 - E
    {0xfe, 0x90, 0x90, 0x90, 0x80}, //  70 - F
    {0x7c, 0x82, 0x92, 0x92, 0x5e}, //  71 - G
    {0xfe, 0x10, 0x10, 0x10, 0xfe}, //  72 - H
    {0x00, 0x82, 0xfe, 0x82, 0x00}, //  73 - I
    {0x04, 0x82, 0x82, 0xfc, 0x00}, //  74 - J
    {0xfe, 0x10, 0x28, 0x44, 0x82}, //  75 - K
    {0xfe, 0x02, 0x02, 0x02, 0x02}, //  76 - L
    {0xfe, 0x40, 0x30, 0x40, 0xfe}, //  77 - M
    {0xfe, 0x20, 0x10, 0x08, 0xfe}, //  78 - N
    {0x7c, 0x82, 0x82, 0x82, 0x7c}, //  79 - O
    {0xfe, 0x90, 0x90, 0x90, 0x60}, //  80 - P
    {0x7c, 0x82, 0x8a, 0x84, 0x7a}, //  81 - Q
    {0xfe, 0x90, 0x98, 0x94, 0x62}, //  82 - R
    {0x62, 0x92, 0x92, 0x92, 0x8c}, //  83 - S
    {0x80, 0x80, 0xfe, 0x80, 0x80}, //  84 - T
    {0xfc, 0x02, 0x02, 0x02, 0xfc}, //  85 - U
    {0xf8, 0x04, 0x02, 0x04, 0xf8}, //  86 - V
    {0xfc, 0x02, 0x1c, 0x02, 0xfc}, //  87 - W
    {0xc6, 0x28, 0x10, 0x28, 0xc6}, //  88 - X
    {0xe0, 0x10, 0x0e, 0x10, 0xe0}, //  89 - Y
    {0x86, 0x8a, 0x92, 0xa2, 0xc2}, //  90 - Z
    {0x00, 0xfe, 0x82, 0x82, 0x00}, //  91 - [
    {0xa8, 0x68, 0x3e, 0x68, 0xa8}, //  92 - fwd slash
    {0x00, 0x82, 0x82, 0xfe, 0x00}, //  93 - ]
    {0x20, 0x40, 0x80, 0x40, 0x20}, //  94 - ^
    {0x02, 0x02, 0x02, 0x02, 0x02}, //  95 - _
    {0x00, 0x80, 0x40, 0x20, 0x00}, //  96 - `
    {0x04, 0x2a, 0x2a, 0x2a, 0x1e}, //  97 - a
    {0xfe, 0x12, 0x22, 0x22, 0x1c}, //  98 - b
    {0x1c, 0x22, 0x22, 0x22, 0x04}, //  99 - c
    {0x1c, 0x22, 0x22, 0x12, 0xfe}, // 100 - d
    {0x1c, 0x2a, 0x2a, 0x2a, 0x18}, // 101 - e
    {0x10, 0x7e, 0x90, 0x80, 0x40}, // 102 - f
    {0x30, 0x4a, 0x4a, 0x4a, 0x7c}, // 103 - g
    {0xfe, 0x10, 0x20, 0x20, 0x1e}, // 104 - h
    {0x00, 0x22, 0xbe, 0x02, 0x00}, // 105 - i
    {0x04, 0x02, 0x22, 0xbc, 0x00}, // 106 - j
    {0xfe, 0x08, 0x14, 0x22, 0x00}, // 107 - k
    {0x02, 0x82, 0xfe, 0x02, 0x02}, // 108 - l
    {0x3e, 0x20, 0x18, 0x20, 0x1e}, // 109 - m
    {0x3e, 0x10, 0x20, 0x20, 0x1e}, // 110 - n
    {0x1c, 0x22, 0x22, 0x22, 0x1c}, // 111 - o
    {0x3e, 0x28, 0x28, 0x28, 0x10}, // 112 - p
    {0x10, 0x28, 0x28, 0x18, 0x3e}, // 113 - q
    {0x3e, 0x10, 0x20, 0x20, 0x10}, // 114 - r
    {0x12, 0x2a, 0x2a, 0x2a, 0x04}, // 115 - s
    {0x20, 0xfc, 0x22, 0x02, 0x04}, // 116 - t
    {0x3c, 0x02, 0x02, 0x04, 0x3e}, // 117 - u
    {0x38, 0x04, 0x02, 0x04, 0x38}, // 118 - v
    {0x3c, 0x02, 0x0c, 0x02, 0x3c}, // 119 - w
    {0x22, 0x14, 0x08, 0x14, 0x22}, // 120 - x
    {0x30, 0x0a, 0x0a, 0x0a, 0x3c}, // 121 - y
    {0x22, 0x26, 0x2a, 0x32, 0x22}, // 122 - z
    {0x00, 0x10, 0x6c, 0x82, 0x00}, // 123 - {
    {0x00, 0x00, 0xfe, 0x00, 0x00}, // 124 - |
    {0x00, 0x82, 0x6c, 0x10, 0x00}, // 125 - }
    {0x10, 0x10, 0x54, 0x38, 0x10}, // 126 - ~
    {0x10, 0x38, 0x54, 0x10, 0x10}, // 127 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 128 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 129 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 130 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 131 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 132 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 133 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 134 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 135 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 136 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 137 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 138 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 139 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 140 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 141 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 142 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 143 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 144 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 145 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 146 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 147 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 148 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 149 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 150 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 151 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 152 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 153 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 154 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 155 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 156 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 157 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 158 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 159 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 160 -
    {0x0e, 0x0a, 0x0e, 0x00, 0x00}, // 161 -
    {0x00, 0x00, 0xf0, 0x80, 0x80}, // 162 -
    {0x02, 0x02, 0x1e, 0x00, 0x00}, // 163 -
    {0x08, 0x04, 0x02, 0x00, 0x00}, // 164 -
    {0x00, 0x18, 0x18, 0x00, 0x00}, // 165 -
    {0x50, 0x50, 0x52, 0x54, 0x78}, // 166 -
    {0x20, 0x22, 0x2c, 0x28, 0x30}, // 167 -
    {0x04, 0x08, 0x1e, 0x20, 0x00}, // 168 -
    {0x18, 0x12, 0x32, 0x12, 0x1c}, // 169 -
    {0x12, 0x12, 0x1e, 0x12, 0x12}, // 170 -
    {0x12, 0x14, 0x18, 0x3e, 0x10}, // 171 -
    {0x10, 0x3e, 0x10, 0x14, 0x18}, // 172 -
    {0x02, 0x12, 0x12, 0x1e, 0x02}, // 173 -
    {0x2a, 0x2a, 0x2a, 0x3e, 0x00}, // 174 -
    {0x18, 0x00, 0x1a, 0x02, 0x1c}, // 175 -
    {0x10, 0x10, 0x10, 0x10, 0x10}, // 176 -
    {0x80, 0x82, 0xbc, 0x90, 0xe0}, // 177 -
    {0x08, 0x10, 0x3e, 0x40, 0x80}, // 178 -
    {0x70, 0x40, 0xc2, 0x44, 0x78}, // 179 -
    {0x42, 0x42, 0x7e, 0x42, 0x42}, // 180 -
    {0x44, 0x48, 0x50, 0xfe, 0x40}, // 181 -
    {0x42, 0xfc, 0x40, 0x42, 0x7c}, // 182 -
    {0x50, 0x50, 0xfe, 0x50, 0x50}, // 183 -
    {0x10, 0x62, 0x42, 0x44, 0x78}, // 184 -
    {0x20, 0xc0, 0x42, 0x7c, 0x40}, // 185 -
    {0x42, 0x42, 0x42, 0x42, 0x7e}, // 186 -
    {0x40, 0xf2, 0x44, 0xf8, 0x40}, // 187 -
    {0x52, 0x52, 0x02, 0x04, 0x38}, // 188 -
    {0x42, 0x44, 0x48, 0x54, 0x62}, // 189 -
    {0x40, 0xfc, 0x42, 0x52, 0x62}, // 190 -
    {0x60, 0x12, 0x02, 0x04, 0x78}, // 191 -
    {0x10, 0x62, 0x52, 0x4c, 0x78}, // 192 -
    {0x50, 0x52, 0x7c, 0x90, 0x10}, // 193 -
    {0x70, 0x00, 0x72, 0x04, 0x78}, // 194 -
    {0x20, 0xa2, 0xbc, 0xa0, 0x20}, // 195 -
    {0x00, 0xfe, 0x10, 0x08, 0x00}, // 196 -
    {0x22, 0x24, 0xf8, 0x20, 0x20}, // 197 -
    {0x02, 0x42, 0x42, 0x42, 0x02}, // 198 -
    {0x42, 0x54, 0x48, 0x54, 0x60}, // 199 -
    {0x44, 0x48, 0xde, 0x68, 0x44}, // 200 -
    {0x00, 0x02, 0x04, 0xf8, 0x00}, // 201 -
    {0x1e, 0x00, 0x40, 0x20, 0x1e}, // 202 -
    {0xfc, 0x22, 0x22, 0x22, 0x22}, // 203 -
    {0x40, 0x42, 0x42, 0x44, 0x78}, // 204 -
    {0x20, 0x40, 0x20, 0x10, 0x0c}, // 205 -
    {0x4c, 0x40, 0xfe, 0x40, 0x4c}, // 206 -
    {0x40, 0x48, 0x44, 0x4a, 0x70}, // 207 -
    {0x00, 0x54, 0x54, 0x54, 0x02}, // 208 -
    {0x1c, 0x24, 0x44, 0x04, 0x0e}, // 209 -
    {0x02, 0x14, 0x08, 0x14, 0x60}, // 210 -
    {0x50, 0x7c, 0x52, 0x52, 0x52}, // 211 -
    {0x20, 0xfe, 0x20, 0x28, 0x30}, // 212 -
    {0x02, 0x42, 0x42, 0x7e, 0x02}, // 213 -
    {0x52, 0x52, 0x52, 0x52, 0x7e}, // 214 -
    {0x20, 0xa0, 0xa2, 0xa4, 0x38}, // 215 -
    {0xf0, 0x02, 0x04, 0xf8, 0x00}, // 216 -
    {0x3e, 0x00, 0x7e, 0x02, 0x0c}, // 217 -
    {0x7e, 0x02, 0x04, 0x08, 0x10}, // 218 -
    {0x7e, 0x42, 0x42, 0x42, 0x7e}, // 219 -
    {0x70, 0x40, 0x42, 0x44, 0x78}, // 220 -
    {0x42, 0x42, 0x02, 0x04, 0x18}, // 221 -
    {0x40, 0x20, 0x80, 0x40, 0x00}, // 222 -
    {0xe0, 0xa0, 0xe0, 0x00, 0x00}, // 223 -
    {0x1c, 0x22, 0x12, 0x0c, 0x32}, // 224 -
    {0x04, 0xaa, 0x2a, 0xaa, 0x1e}, // 225 -
    {0x1f, 0x2a, 0x2a, 0x2a, 0x14}, // 226 -
    {0x14, 0x2a, 0x2a, 0x22, 0x04}, // 227 -
    {0x3f, 0x02, 0x02, 0x04, 0x3e}, // 228 -
    {0x1c, 0x22, 0x32, 0x2a, 0x24}, // 229 -
    {0x0f, 0x12, 0x22, 0x22, 0x1c}, // 230 -
    {0x1c, 0x22, 0x22, 0x22, 0x3f}, // 231 -
    {0x04, 0x02, 0x3c, 0x20, 0x20}, // 232 -
    {0x20, 0x20, 0x00, 0x70, 0x00}, // 233 -
    {0x00, 0x00, 0x20, 0xbf, 0x00}, // 234 -
    {0x50, 0x20, 0x50, 0x00, 0x00}, // 235 -
    {0x18, 0x24, 0x7e, 0x24, 0x08}, // 236 -
    {0x28, 0xfe, 0x2a, 0x02, 0x02}, // 237 -
    {0x3e, 0x90, 0xa0, 0xa0, 0x1e}, // 238 -
    {0x1c, 0xa2, 0x22, 0xa2, 0x1c}, // 239 -
    {0x3f, 0x12, 0x22, 0x22, 0x1c}, // 240 -
    {0x1c, 0x22, 0x22, 0x12, 0x3f}, // 241 -
    {0x3c, 0x52, 0x52, 0x52, 0x3c}, // 242 -
    {0x0c, 0x14, 0x08, 0x14, 0x18}, // 243 -
    {0x1a, 0x26, 0x20, 0x26, 0x1a}, // 244 -
    {0x3c, 0x82, 0x02, 0x84, 0x3e}, // 245 -
    {0xc6, 0xaa, 0x92, 0x82, 0x82}, // 246 -
    {0x22, 0x3c, 0x20, 0x3e, 0x22}, // 247 -
    {0xa2, 0x94, 0x88, 0x94, 0xa2}, // 248 -
    {0x3c, 0x02, 0x02, 0x02, 0x3f}, // 249 -
    {0x28, 0x28, 0x3e, 0x28, 0x48}, // 250 -
    {0x22, 0x3c, 0x28, 0x28, 0x2e}, // 251 -
    {0x3e, 0x28, 0x38, 0x28, 0x3e}, // 252 -
    {0x08, 0x08, 0x2a, 0x08, 0x08}, // 253 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 254 -
    {0xff, 0xff, 0xff, 0xff, 0xff}  // 255 -
};


// A02 (European) character set.
// skip first 16 characters reserved for CGRAM
const uint8_t LCDWidget::fontA02[ROM_FONT_CHARS][LCD_CHAR_W] = {
    {0x00, 0x7f, 0x3e, 0x1c, 0x08}, //  16 -
    {0x08, 0x1c, 0x3e, 0x7f, 0x00}, //  17 -
    {0x30, 0x50, 0x00, 0x30, 0x50}, //  18 -
    {0x50, 0x60, 0x00, 0x50, 0x60}, //  19 -
    {0x11, 0x33, 0x77, 0x33, 0x11}, //  20 -
    {0x44, 0x66, 0x77, 0x66, 0x44}, //  21 -
    {0x1c, 0x3e, 0x3e, 0x3e, 0x1c}, //  22 -
    {0x04, 0x0e, 0x15, 0x04, 0x7c}, //  23 -
    {0x10, 0x20, 0x7f, 0x20, 0x10}, //  24 -
    {0x04, 0x02, 0x7f, 0x02, 0x04}, //  25 -
    {0x08, 0x08, 0x2a, 0x1c, 0x08}, //  26 -
    {0x08, 0x1c, 0x2a, 0x08, 0x08}, //  27 -
    {0x01, 0x11, 0x29, 0x45, 0x01}, //  28 -
    {0x01, 0x45, 0x29, 0x11, 0x01}, //  29 -
    {0x02, 0x0e, 0x3e, 0x0e, 0x02}, //  30 -
    {0x20, 0x38, 0x3e, 0x38, 0x20}, //  31 -
    {0x00, 0x00, 0x00, 0x00, 0x00}, //  32 -
    {0x00, 0x00, 0x79, 0x00, 0x00}, //  33 - !
    {0x00, 0x70, 0x00, 0x70, 0x00}, //  34 - "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, //  35 - #
    {0x12, 0x2a, 0x7f, 0x2a, 0x24}, //  36 - $
    {0x62, 0x64, 0x08, 0x13, 0x23}, //  37 - %
    {0x36, 0x49, 0x55, 0x22, 0x05}, //  38 - &
    {0x00, 0x50, 0x60, 0x00, 0x00}, //  39 - '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, //  40 - (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, //  41 - )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, //  42 - *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, //  43 - +
    {0x00, 0x05, 0x06, 0x00, 0x00}, //  44 - ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, //  45 - -
    {0x00, 0x03, 0x03, 0x00, 0x00}, //  46 - .
    {0x02, 0x04, 0x08, 0x10, 0x20}, //  47 - /
    {0x3e, 0x45, 0x49, 0x51, 0x3e}, //  48 - 0
    {0x00, 0x21, 0x7f, 0x01, 0x00}, //  49 - 1
    {0x21, 0x43, 0x45, 0x49, 0x31}, //  50 - 2
    {0x42, 0x41, 0x51, 0x69, 0x46}, //  51 - 3
    {0x0c, 0x14, 0x24, 0x7f, 0x04}, //  52 - 4
    {0x72, 0x51, 0x51, 0x51, 0x4e}, //  53 - 5
    {0x1e, 0x29, 0x49, 0x49, 0x06}, //  54 - 6
    {0x40, 0x47, 0x48, 0x50, 0x60}, //  55 - 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, //  56 - 8
    {0x30, 0x49, 0x49, 0x4a, 0x3c}, //  57 - 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, //  58 - :
    {0x00, 0x35, 0x36, 0x00, 0x00}, //  59 - ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, //  60 - <
    {0x14, 0x14, 0x14, 0x14, 0x14}, //  61 - =
    {0x00, 0x41, 0x22, 0x14, 0x08}, //  62 - >
    {0x20, 0x40, 0x45, 0x48, 0x30}, //  63 - ?
    {0x26, 0x49, 0x4f, 0x41, 0x3e}, //  64 - @
    {0x1f, 0x24, 0x44, 0x24, 0x1f}, //  65 - A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, //  66 - B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, //  67 - C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, //  68 - D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, //  69 - E
    {0x7f, 0x48, 0x48, 0x48, 0x40}, //  70 - F
    {0x3e, 0x41, 0x49, 0x49, 0x2f}, //  71 - G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, //  72 - H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, //  73 - I
    {0x02, 0x41, 0x41, 0x7e, 0x00}, //  74 - J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, //  75 - K
    {0x7f, 0x01, 0x01, 0x01, 0x01}, //  76 - L
    {0x7f, 0x20, 0x18, 0x20, 0x7f}, //  77 - M
    {0x7f, 0x10, 0x08, 0x04, 0x7f}, //  78 - N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, //  79 - O
    {0x7f, 0x48, 0x48, 0x48, 0x30}, //  80 - P
    {0x3e, 0x41, 0x45, 0x42, 0x3d}, //  81 - Q
    {0x7f, 0x48, 0x4c, 0x4a, 0x31}, //  82 - R
    {0x31, 0x49, 0x49, 0x49, 0x46}, //  83 - S
    {0x40, 0x40, 0x7f, 0x40, 0x40}, //  84 - T
    {0x7e, 0x01, 0x01, 0x01, 0x7e}, //  85 - U
    {0x7c, 0x02, 0x01, 0x02, 0x7c}, //  86 - V
    {0x7e, 0x01, 0x0e, 0x01, 0x7e}, //  87 - W
    {0x63, 0x14, 0x08, 0x14, 0x63}, //  88 - X
    {0x70, 0x08, 0x07, 0x08, 0x70}, //  89 - Y
    {0x43, 0x45, 0x49, 0x51, 0x61}, //  90 - Z
    {0x00, 0x7f, 0x41, 0x41, 0x00}, //  91 - [
    {0x20, 0x10, 0x08, 0x04, 0x02}, //  92 - fwd slash
    {0x00, 0x41, 0x41, 0x7f, 0x00}, //  93 - ]
    {0x10, 0x20, 0x40, 0x20, 0x10}, //  94 - ^
    {0x01, 0x01, 0x01, 0x01, 0x01}, //  95 - _
    {0x00, 0x40, 0x20, 0x10, 0x00}, //  96 - `
    {0x02, 0x15, 0x15, 0x15, 0x0f}, //  97 - a
    {0x7f, 0x09, 0x11, 0x11, 0x0e}, //  98 - b
    {0x0e, 0x11, 0x11, 0x11, 0x02}, //  99 - c
    {0x0e, 0x11, 0x11, 0x09, 0x7f}, // 100 - d
    {0x0e, 0x15, 0x15, 0x15, 0x0c}, // 101 - e
    {0x08, 0x3f, 0x48, 0x40, 0x20}, // 102 - f
    {0x18, 0x25, 0x25, 0x25, 0x3e}, // 103 - g
    {0x7f, 0x08, 0x10, 0x10, 0x0f}, // 104 - h
    {0x00, 0x09, 0x5f, 0x01, 0x00}, // 105 - i
    {0x02, 0x01, 0x11, 0x5e, 0x00}, // 106 - j
    {0x7f, 0x04, 0x0a, 0x11, 0x00}, // 107 - k
    {0x01, 0x41, 0x7f, 0x01, 0x01}, // 108 - l
    {0x1f, 0x10, 0x0c, 0x10, 0x0f}, // 109 - m
    {0x1f, 0x08, 0x10, 0x10, 0x0f}, // 110 - n
    {0x0e, 0x11, 0x11, 0x11, 0x0e}, // 111 - o
    {0x1f, 0x14, 0x14, 0x14, 0x08}, // 112 - p
    {0x08, 0x14, 0x14, 0x0c, 0x1f}, // 113 - q
    {0x1f, 0x08, 0x10, 0x10, 0x08}, // 114 - r
    {0x09, 0x15, 0x15, 0x15, 0x02}, // 115 - s
    {0x10, 0x7e, 0x11, 0x01, 0x02}, // 116 - t
    {0x1e, 0x01, 0x01, 0x02, 0x1f}, // 117 - u
    {0x1c, 0x02, 0x01, 0x02, 0x1c}, // 118 - v
    {0x1e, 0x01, 0x06, 0x01, 0x1e}, // 119 - w
    {0x11, 0x0a, 0x04, 0x0a, 0x11}, // 120 - x
    {0x18, 0x05, 0x05, 0x05, 0x1e}, // 121 - y
    {0x11, 0x13, 0x15, 0x19, 0x11}, // 122 - z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // 123 - {
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // 124 - |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // 125 - }
    {0x04, 0x08, 0x08, 0x04, 0x08}, // 126 - ~
    {0x1e, 0x22, 0x42, 0x22, 0x1e}, // 127 -
    {0x7f, 0x49, 0x49, 0x49, 0x66}, // 128 -
    {0x0f, 0x94, 0xe4, 0x84, 0xff}, // 129 -
    {0x77, 0x08, 0x7f, 0x08, 0x77}, // 130 -
    {0x41, 0x41, 0x49, 0x49, 0x36}, // 131 -
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 132 -
    {0x3f, 0x84, 0x48, 0x90, 0x3f}, // 133 -
    {0x02, 0x41, 0x7e, 0x40, 0x7f}, // 134 -
    {0x7f, 0x40, 0x40, 0x40, 0x7f}, // 135 -
    {0x71, 0x0a, 0x04, 0x08, 0x70}, // 136 -
    {0x7e, 0x02, 0x02, 0x02, 0x7f}, // 137 -
    {0x70, 0x08, 0x08, 0x08, 0x7f}, // 138 -
    {0x3f, 0x01, 0x3f, 0x01, 0x3f}, // 139 -
    {0x7e, 0x02, 0x7e, 0x02, 0x7f}, // 140 -
    {0x40, 0x7f, 0x09, 0x09, 0x06}, // 141 -
    {0x7f, 0x09, 0x06, 0x00, 0x7f}, // 142 -
    {0x22, 0x49, 0x51, 0x49, 0x3e}, // 143 -
    {0x0e, 0x11, 0x09, 0x06, 0x19}, // 144 -
    {0x03, 0x03, 0x7f, 0x20, 0x18}, // 145 -
    {0x7f, 0x40, 0x40, 0x40, 0x60}, // 146 -
    {0x11, 0x1e, 0x10, 0x1f, 0x11}, // 147 -
    {0x63, 0x55, 0x49, 0x41, 0x41}, // 148 -
    {0x0e, 0x11, 0x11, 0x1e, 0x10}, // 149 -
    {0x06, 0x06, 0xfc, 0xa3, 0x7f}, // 150 -
    {0x08, 0x10, 0x1e, 0x11, 0x20}, // 151 -
    {0x04, 0x3c, 0x7e, 0x3c, 0x04}, // 152 -
    {0x3e, 0x49, 0x49, 0x49, 0x3e}, // 153 -
    {0x1d, 0x23, 0x20, 0x23, 0x1d}, // 154 -
    {0x06, 0x29, 0x51, 0x49, 0x26}, // 155 -
    {0x0c, 0x14, 0x08, 0x14, 0x18}, // 156 -
    {0x1c, 0x3e, 0x1f, 0x3e, 0x1c}, // 157 -
    {0x0a, 0x15, 0x15, 0x11, 0x02}, // 158 -
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 159 -
    {0x7f, 0x7f, 0x00, 0x7f, 0x7f}, // 160 -
    {0x00, 0x00, 0x4f, 0x00, 0x00}, // 161 - ¡
    {0x1c, 0x22, 0x7f, 0x22, 0x04}, // 162 - ¢
    {0x09, 0x3e, 0x49, 0x41, 0x02}, // 163 - £
    {0x22, 0x1c, 0x14, 0x1c, 0x22}, // 164 - ¤
    {0x54, 0x34, 0x1f, 0x34, 0x54}, // 165 - ¥
    {0x00, 0x00, 0x77, 0x00, 0x00}, // 166 - ¦
    {0x02, 0x29, 0x55, 0x4a, 0x20}, // 167 - §
    {0x0a, 0x09, 0x3e, 0x48, 0x28}, // 168 - ¨
    {0x7f, 0x41, 0x5d, 0x49, 0x7f}, // 169 - ©
    {0x09, 0x55, 0x55, 0x55, 0x3d}, // 170 - ª
    {0x08, 0x14, 0x2a, 0x14, 0x22}, // 171 - «
    {0x7f, 0x08, 0x3e, 0x41, 0x3e}, // 172 - ¬
    {0x31, 0x4a, 0x4c, 0x48, 0x7f}, // 173 - ­
    {0x7f, 0x41, 0x53, 0x45, 0x7f}, // 174 - ®
    {0x00, 0x30, 0x50, 0x00, 0x00}, // 175 - ¯
    {0x70, 0x88, 0x88, 0x70, 0x00}, // 176 - °
    {0x11, 0x11, 0x7d, 0x11, 0x11}, // 177 - ±
    {0x48, 0x98, 0xa8, 0x48, 0x00}, // 178 - ²
    {0x88, 0xa8, 0xa8, 0x50, 0x00}, // 179 - ³
    {0xfe, 0xa0, 0xa4, 0x4f, 0x05}, // 180 - ´
    {0x7f, 0x04, 0x04, 0x08, 0x7c}, // 181 - µ
    {0x30, 0x48, 0x48, 0x7f, 0x7f}, // 182 - ¶
    {0x00, 0x0c, 0x0c, 0x00, 0x00}, // 183 - ·
    {0x0e, 0x11, 0x06, 0x11, 0x0e}, // 184 - ¸
    {0x48, 0xf8, 0x08, 0x00, 0x00}, // 185 - ¹
    {0x39, 0x45, 0x45, 0x45, 0x39}, // 186 - º
    {0x22, 0x14, 0x2a, 0x14, 0x08}, // 187 - »
    {0xe8, 0x16, 0x2a, 0x5f, 0x82}, // 188 - ¼
    {0xe8, 0x10, 0x29, 0x53, 0x8d}, // 189 - ½
    {0xa8, 0xf8, 0x06, 0x0a, 0x1f}, // 190 - ¾
    {0x06, 0x09, 0x51, 0x01, 0x02}, // 191 - ¿
    {0x0f, 0x94, 0x64, 0x14, 0x0f}, // 192 - À
    {0x0f, 0x14, 0x64, 0x94, 0x0f}, // 193 - Á
    {0x0f, 0x54, 0x94, 0x54, 0x0f}, // 194 - Â
    {0x4f, 0x94, 0x94, 0x54, 0x8f}, // 195 - Ã
    {0x0f, 0x94, 0x24, 0x94, 0x0f}, // 196 - Ä
    {0x0f, 0x54, 0xa4, 0x54, 0x0f}, // 197 - Å
    {0x1f, 0x24, 0x7f, 0x49, 0x49}, // 198 - Æ
    {0x78, 0x84, 0x85, 0x87, 0x48}, // 199 - Ç
    {0x1f, 0x95, 0x55, 0x15, 0x11}, // 200 - È
    {0x1f, 0x15, 0x55, 0x95, 0x11}, // 201 - É
    {0x1f, 0x55, 0x95, 0x55, 0x11}, // 202 - Ê
    {0x1f, 0x55, 0x15, 0x55, 0x11}, // 203 - Ë
    {0x00, 0x91, 0x5f, 0x11, 0x00}, // 204 - Ì
    {0x00, 0x11, 0x5f, 0x91, 0x00}, // 205 - Í
    {0x00, 0x51, 0x9f, 0x51, 0x00}, // 206 - Î
    {0x00, 0x51, 0x1f, 0x51, 0x00}, // 207 - Ï
    {0x08, 0x7f, 0x49, 0x41, 0x3e}, // 208 - Ð
    {0x5f, 0x88, 0x84, 0x42, 0x9f}, // 209 - Ñ
    {0x1e, 0xa1, 0x61, 0x21, 0x1e}, // 210 - Ò
    {0x1e, 0x21, 0x61, 0xa1, 0x1e}, // 211 - Ó
    {0x0e, 0x51, 0x91, 0x51, 0x0e}, // 212 - Ô
    {0x4e, 0x91, 0x91, 0x51, 0x8e}, // 213 - Õ
    {0x1e, 0xa1, 0x21, 0xa1, 0x1e}, // 214 - Ö
    {0x22, 0x14, 0x08, 0x14, 0x22}, // 215 - ×
    {0x08, 0x55, 0x7f, 0x55, 0x08}, // 216 - Ø
    {0x3e, 0x81, 0x41, 0x01, 0x3e}, // 217 - Ù
    {0x3e, 0x01, 0x41, 0x81, 0x3e}, // 218 - Ú
    {0x1e, 0x41, 0x81, 0x41, 0x1e}, // 219 - Û
    {0x3e, 0x81, 0x01, 0x81, 0x3e}, // 220 - Ü
    {0x20, 0x10, 0x4f, 0x90, 0x20}, // 221 - Ý
    {0x81, 0xff, 0x25, 0x24, 0x18}, // 222 - Þ
    {0x01, 0x3e, 0x49, 0x49, 0x36}, // 223 - ß
    {0x02, 0x95, 0x55, 0x15, 0x0f}, // 224 - à
    {0x02, 0x15, 0x55, 0x95, 0x0f}, // 225 - á
    {0x02, 0x55, 0x95, 0x55, 0x0f}, // 226 - â
    {0x42, 0x95, 0x95, 0x55, 0x8f}, // 227 - ã
    {0x02, 0x55, 0x15, 0x55, 0x0f}, // 228 - ä
    {0x02, 0x55, 0xb5, 0x55, 0x0f}, // 229 - å
    {0x26, 0x29, 0x1e, 0x29, 0x1a}, // 230 - æ
    {0x18, 0x25, 0x27, 0x24, 0x08}, // 231 - ç
    {0x0e, 0x95, 0x55, 0x15, 0x0c}, // 232 - è
    {0x0e, 0x15, 0x55, 0x95, 0x0c}, // 233 - é
    {0x0e, 0x55, 0x95, 0x55, 0x0c}, // 234 - ê
    {0x0e, 0x55, 0x15, 0x55, 0x0c}, // 235 - ë
    {0x00, 0x89, 0x5f, 0x01, 0x00}, // 236 - ì
    {0x00, 0x09, 0x5f, 0x81, 0x00}, // 237 - í
    {0x00, 0x49, 0x9f, 0x41, 0x00}, // 238 - î
    {0x00, 0x49, 0x1f, 0x41, 0x00}, // 239 - ï
    {0x52, 0x25, 0x55, 0x0d, 0x06}, // 240 - ð
    {0x5f, 0x88, 0x90, 0x50, 0x8f}, // 241 - ñ
    {0x0e, 0x91, 0x51, 0x11, 0x0e}, // 242 - ò
    {0x0e, 0x11, 0x51, 0x91, 0x0e}, // 243 - ó
    {0x06, 0x29, 0x49, 0x29, 0x06}, // 244 - ô
    {0x26, 0x49, 0x49, 0x29, 0x46}, // 245 - õ
    {0x0e, 0x51, 0x11, 0x51, 0x0e}, // 246 - ö
    {0x08, 0x08, 0x2a, 0x08, 0x08}, // 247 - ÷
    {0x08, 0x15, 0x3e, 0x54, 0x08}, // 248 - ø
    {0x1e, 0x81, 0x41, 0x02, 0x1f}, // 249 - ù
    {0x1e, 0x01, 0x41, 0x82, 0x1f}, // 250 - ú
    {0x1e, 0x41, 0x81, 0x42, 0x1f}, // 251 - û
    {0x1e, 0x41, 0x01, 0x42, 0x1f}, // 252 - ü
    {0x18, 0x05, 0x45, 0x85, 0x1e}, // 253 - ý
    {0x00, 0x41, 0x7f, 0x15, 0x08}, // 254 - þ
    {0x18, 0x45, 0x05, 0x45, 0x1e}, // 255 - ÿ
};
