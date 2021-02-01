#include "random.h"

// ported from https://github.com/protohaus/ESPRandom which actually seems to be ESP32 only?

// #define USE_RNG 1

uint32_t ESPRandom::get()
{
    //   enableRadio();
    //   return esp_random();

    //   uint32_t bit = USE_RNG
    //                     ? (int)RANDOM_REG32 //using the onboard hardware random number generator (esp8266_peri.h)
    //                     : analogRead(A0);   //using A0 / TOUT

    //                     return bit;

    return (int)RANDOM_REG32;
}

void ESPRandom::uuid(uint8_t *buffer)
{
    for (int i = 0; i < 16; i += 4)
    {
        uint32_t random = get();
        memcpy(&buffer[i], &random, 4);
    }

    // Format the UUID --> version number and control bits
    buffer[6] = (buffer[6] & 0x0F) | 0x40;
    buffer[8] = (buffer[8] & 0x3F) | 0x80;
}

std::vector<uint8_t> ESPRandom::uuid()
{
    std::vector<uint8_t> buffer = std::vector<uint8_t>(16);
    ESPRandom::uuid(&buffer.front());
    return buffer;
}

String ESPRandom::uuidToString(const uint8_t *buffer)
{
    String uuid_string;
    uuid_string.reserve(36 + 1); // Include NULL

    // Convert to HEX and break up into blocks (8, 4, 4, 4, 12)
    for (int i = 0; i < 16; i++)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
        {
            uuid_string += "-";
        }
        uuid_string += String(buffer[i] >> 4, HEX);
        uuid_string += String(buffer[i] & 0x0F, HEX);
    }

    return uuid_string;
}

String ESPRandom::uuidToString(const std::vector<uint8_t> &buffer)
{
    if (buffer.size() != 16)
    {
        return String();
    }

    return uuidToString(&buffer.front());
}

bool ESPRandom::isValidV4Uuid(const uint8_t *buffer)
{
    // The 13th hex has to be 4
    if (buffer[6] >> 4 != 0x4)
    {
        return false;
    }

    // The 17th hex has to by 0x8, 0x9, 0xA or 0xB
    uint8_t hex17 = buffer[8] >> 4;
    if (hex17 != 0x8 && hex17 != 0x9 && hex17 != 0xA && hex17 != 0xB)
    {
        return false;
    }

    return true;
}

bool ESPRandom::isValidV4Uuid(const std::vector<uint8_t> &buffer)
{
    if (buffer.size() == 16)
    {
        return isValidV4Uuid(&buffer.front());
    }
    else
    {
        return false;
    }
}

// void ESPRandom::enableRadio() {
//   if (WiFi.getMode() == WIFI_MODE_NULL) {
//     WiFi.begin();
//   }
// }