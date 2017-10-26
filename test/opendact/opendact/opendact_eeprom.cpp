#include "opendact_eeprom.h"
#include "common.h"
#include "opendact.h"
#include <QStringList>

COpendactEeprom::COpendactEeprom()
{
    stepsPerMM   = 100;
    tempSPM      = stepsPerMM;
    zMaxLength   = 300;
    zProbeHeight = 0;
    zProbeSpeed  = 5;
    HRadius      = 95;
    diagonalRod  = 217;
    offsetX      = 0;
    offsetY      = 0;
    offsetZ      = 0;
    A            = 210;
    B            = 330;
    C            = 90;
    DA           = 0;
    DB           = 0;
    DC           = 0;

    tempEEPROMSet  = false;
}

void COpendactEeprom::readEEPROM()
{
    CEePromRecord* record;
    record = Printer.EEPROM->atByStartName("Steps per mm");
    if (record) {stepsPerMM = record->FValue; tempSPM = stepsPerMM;}

    record = Printer.EEPROM->atByStartName("Z max length");
    if (record) zMaxLength = record->FValue;

    record = Printer.EEPROM->atByStartName("Z-probe height");
    if (record) zProbeHeight = record->FValue;

    record = Printer.EEPROM->atByStartName("Z-probe speed");
    if (record) zProbeSpeed = record->FValue;

    record = Printer.EEPROM->atByStartName("Horizontal rod radius at");
    if (record) HRadius = record->FValue;
    record = Printer.EEPROM->atByStartName("Diagonal rod length");
    if (record) diagonalRod = record->FValue;

    record = Printer.EEPROM->atByStartName("Tower X endstop offset");
    if (record) offsetX = record->IValue;
    record = Printer.EEPROM->atByStartName("Tower Y endstop offset");
    if (record) offsetY = record->IValue;
    record = Printer.EEPROM->atByStartName("Tower Z endstop offset");
    if (record) offsetZ = record->IValue;

    record = Printer.EEPROM->atByStartName("Alpha A");
    if (record) A = record->FValue;
    record = Printer.EEPROM->atByStartName("Alpha B");
    if (record) B = record->FValue;
    record = Printer.EEPROM->atByStartName("Alpha C");
    if (record) C = record->FValue;

    record = Printer.EEPROM->atByStartName("Delta Radius A");
    if (record) DA = record->FValue;
    record = Printer.EEPROM->atByStartName("Delta Radius B");
    if (record) DB = record->FValue;
    record = Printer.EEPROM->atByStartName("Delta Radius C");
    if (record) DC = record->FValue;
    tempEEPROMSet = true;
}

int COpendactEeprom::sendEEPROM()
{
//    emit OpenDACT.signalLogConsole("Setting EEPROM.");
    QStringList script;
    CEePromRecord* record;
    record = Printer.EEPROM->atByStartName("Steps per mm");
    if (record && stepsPerMM != record->FValue){
        record->FValue = stepsPerMM;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Z max length");
    if (record && zMaxLength != record->FValue){
        record->FValue = zMaxLength;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Z-probe height");
    if (record && zProbeHeight != record->FValue){
        record->FValue = zProbeHeight;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Z-probe speed");
    if (record && zProbeSpeed != record->FValue){
        record->FValue = zProbeSpeed;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Horizontal rod radius at");
    if (record && HRadius != record->FValue){
        record->FValue = HRadius;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Tower X endstop offset");
    if (record && offsetX != record->IValue){
        record->IValue = offsetX;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Tower Y endstop offset");
    if (record && offsetY != record->IValue){
        record->IValue = offsetY;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Tower Z endstop offset");
    if (record && offsetZ != record->IValue){
        record->IValue = offsetZ;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Alpha A");
    if (record && A != record->FValue){
        record->FValue = A;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Alpha B");
    if (record && B != record->FValue){
        record->FValue = B;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Alpha C");
    if (record && C != record->FValue){
        record->FValue = C;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Delta Radius A");
    if (record && DA != record->FValue){
        record->FValue = DA;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Delta Radius B");
    if (record && DB != record->FValue){
        record->FValue = DB;
        script.append(record->ToCmdString());
    }
    record = Printer.EEPROM->atByStartName("Delta Radius C");
    if (record && DC != record->FValue){
        record->FValue = DC;
        script.append(record->ToCmdString());
    }
    int len = script.length();
    if (len>0)
        Printer.sendScript(script);
    return len;
}
