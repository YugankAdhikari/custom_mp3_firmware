#include "command.h"

#include <Arduino.h>

#include "player.h"
#include "bluetooth_manager.h"
#include "bluetooth_gap.h"
#include "ringbuffer.h"

Command command;

void Command::begin()
{
    Serial.println();
    Serial.println("===== COMMAND CONSOLE =====");
    Serial.println("Type 'h' for help.");
    Serial.println();
}

void Command::printHelp()
{
    Serial.println();
    Serial.println("========== PLAYER COMMANDS ==========");
    Serial.println("h : Help");
    Serial.println("i : Player Info");
    Serial.println("p : Play / Pause");
    Serial.println("n : Next Track");
    Serial.println("b : Previous Track");
    Serial.println("f : Next Folder");
    Serial.println("r : Previous Folder");
    Serial.println("+ : Volume Up");
    Serial.println("- : Volume Down");
    Serial.println("d : Buffer Diagnostics");
    Serial.println("======== BLUETOOTH COMMANDS ========");
    Serial.println("bt pair     : Enter Pairing Mode");
    Serial.println("bt scan     : Start 10s Discovery");
    Serial.println("bt list     : List discovered devices");
    Serial.println("bt connect X: Connect to device index X");
    Serial.println("bt forget   : Forget saved device");
    Serial.println("bt reconnect: Force reconnection");
    Serial.println("bt status   : Print Bluetooth state");
    Serial.println("=====================================");
}

void Command::update()
{
    if (!Serial.available())
        return;

    // Read full line
    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() == 0) {
        // If we just pressed enter and we are in PAIRING state, trigger discovery
        if (bluetooth.currentState() == BluetoothState::PAIRING) {
            bluetooth.triggerDiscovery();
        }
        return;
    }

    Serial.print("> ");
    Serial.println(line);

    if (line == "h") {
        printHelp();
    }
    else if (line == "i") {
        player.begin();
    }
    else if (line == "p") {
        player.togglePlayPause();
    }
    else if (line == "n") {
        player.nextTrack();
    }
    else if (line == "b") {
        player.previousTrack();
    }
    else if (line == "f") {
        player.nextFolder();
    }
    else if (line == "r") {
        player.previousFolder();
    }
    else if (line == "+") {
        player.volumeUp();
    }
    else if (line == "-") {
        player.volumeDown();
    }
    else if (line == "d") {
        Serial.println("====== BUFFER DIAGNOSTICS ======");
        Serial.printf("Current Fill: %zu samples (%.2f KB)\n", pcmBuffer.available(), pcmBuffer.available() * 2.0 / 1024.0);
        Serial.printf("Peak Fill   : %zu samples (%.2f KB)\n", pcmBuffer.peakFill(), pcmBuffer.peakFill() * 2.0 / 1024.0);
        Serial.printf("Total Size  : %zu samples (%.2f KB)\n", PCMRingBuffer::BUFFER_SAMPLES * 2, (PCMRingBuffer::BUFFER_SAMPLES * 2 * 2.0) / 1024.0);
        Serial.printf("Underruns   : %u\n", pcmBuffer.underruns());
        Serial.printf("Overruns    : %u\n", pcmBuffer.overruns());
        Serial.println("================================");
    }
    else if (line.startsWith("bt ")) {
        String btCmd = line.substring(3);
        btCmd.trim();
        
        if (btCmd == "pair") {
            bluetooth.startPairingMode();
        }
        else if (btCmd == "scan") {
            bluetooth.triggerDiscovery();
        }
        else if (btCmd == "list") {
            int count = bt_gap_get_discovered_device_count();
            DiscoveredDevice* devices = bt_gap_get_discovered_devices();
            Serial.printf("Discovered %d Audio Sink(s):\n", count);
            for (int i = 0; i < count; i++) {
                Serial.printf("[%d] %s\n", i, devices[i].name);
                Serial.printf("    MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                    devices[i].bda[0], devices[i].bda[1], devices[i].bda[2],
                    devices[i].bda[3], devices[i].bda[4], devices[i].bda[5]);
                Serial.printf("    RSSI: %d\n", devices[i].rssi);
            }
        }
        else if (btCmd.startsWith("connect ")) {
            int index = btCmd.substring(8).toInt();
            int count = bt_gap_get_discovered_device_count();
            if (index >= 0 && index < count) {
                DiscoveredDevice* devices = bt_gap_get_discovered_devices();
                bluetooth.connectToDevice(devices[index].bda);
            } else {
                Serial.println("Invalid device index.");
            }
        }
        else if (btCmd == "forget") {
            bluetooth.forgetDevice();
        }
        else if (btCmd == "reconnect") {
            bluetooth.forceReconnect();
        }
        else if (btCmd == "status") {
            Serial.printf("Bluetooth State: %d\n", (int)bluetooth.currentState());
        }
        else {
            Serial.println("Unknown bt command.");
        }
    }
    else {
        Serial.println("Unknown command.");
    }
}

// ======================================================
// Legacy compatibility wrappers (used by encoder.cpp)
// ======================================================

void commandRotateCW()
{
    player.volumeUp();
}

void commandRotateCCW()
{
    player.volumeDown();
}

void commandClick()
{
    player.nextFolder();
}

void commandLongClick()
{
    player.togglePlayPause();
}