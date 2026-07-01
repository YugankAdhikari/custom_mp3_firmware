#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "storage.h"
#include "pins.h"

Storage storage;

SPIClass spi(VSPI);

bool Storage::begin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("        STORAGE");
    Serial.println("==============================");

    Serial.println("Starting SPI...");
    spi.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    Serial.println("SPI configured.");
    Serial.printf("CS   = %d\n", PIN_SD_CS);
    Serial.printf("SCK  = %d\n", PIN_SD_SCK);
    Serial.printf("MISO = %d\n", PIN_SD_MISO);
    Serial.printf("MOSI = %d\n", PIN_SD_MOSI);

    Serial.println("Calling SD.begin()...");

    if (!SD.begin(PIN_SD_CS, spi))
    {
        Serial.println("❌ SD.begin() FAILED");

        uint8_t cardType = SD.cardType();

        Serial.print("SD.cardType() = ");

        switch (cardType)
        {
            case CARD_NONE:
                Serial.println("CARD_NONE (0)");
                break;

            case CARD_MMC:
                Serial.println("MMC");
                break;

            case CARD_SD:
                Serial.println("SDSC");
                break;

            case CARD_SDHC:
                Serial.println("SDHC");
                break;

            default:
                Serial.printf("UNKNOWN (%d)\n", cardType);
                break;
        }

        return false;
    }

    Serial.println("✅ SD.begin() OK");

    uint8_t cardType = SD.cardType();

    Serial.print("Card Type : ");

    switch (cardType)
    {
        case CARD_NONE:
            Serial.println("CARD_NONE");
            break;

        case CARD_MMC:
            Serial.println("MMC");
            break;

        case CARD_SD:
            Serial.println("SDSC");
            break;

        case CARD_SDHC:
            Serial.println("SDHC");
            break;

        default:
            Serial.printf("UNKNOWN (%d)\n", cardType);
            break;
    }

    uint64_t size = SD.cardSize() / (1024 * 1024);

    Serial.printf("Card Size : %llu MB\n", size);

    Serial.println();
    Serial.println("Root Directory");
    Serial.println("----------------");

    File root = SD.open("/");

    if (!root)
    {
        Serial.println("❌ Failed to open root directory.");
        return false;
    }

    while (true)
    {
        File file = root.openNextFile();

        if (!file)
            break;

        if (file.isDirectory())
        {
            Serial.print("[DIR]  ");
            Serial.println(file.name());
        }
        else
        {
            Serial.printf("[FILE] %-20s %d bytes\n",
                          file.name(),
                          file.size());
        }

        file.close();
    }

    root.close();

    Serial.println();
    Serial.println("✅ Storage Ready.");

    return true;
}