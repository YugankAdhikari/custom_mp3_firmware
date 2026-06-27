#pragma once

// ============================================================
// Cyber Cassette
// Hardware Configuration
// ============================================================

// ---------- Buttons ----------
constexpr int PIN_PLAY_PAUSE = 13;
constexpr int PIN_NEXT       = 14;
constexpr int PIN_PREVIOUS   = 27;
constexpr int PIN_FOLDER     = 26;

// ---------- Encoder ----------
constexpr int PIN_ENC_A      = 32;
constexpr int PIN_ENC_B      = 33;
constexpr int PIN_ENC_SW     = 25;

// ---------- Status LED ----------
constexpr int PIN_STATUS_LED = 2;

// ---------- microSD ----------
constexpr int PIN_SD_CS      = 5;

// ---------- I2S DAC ----------
constexpr int PIN_I2S_BCLK   = 26;
constexpr int PIN_I2S_LRCLK  = 25;
constexpr int PIN_I2S_DOUT   = 22;

// ---------- Battery ----------
constexpr int PIN_BATTERY    = 34;

// ---------- Serial ----------
constexpr long SERIAL_BAUD   = 115200;

// ---------- Bluetooth ----------
constexpr char DEVICE_NAME[] = "Cyber Cassette";