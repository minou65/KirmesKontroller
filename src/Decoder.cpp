// 
// 
// 

#include "Decoder.h"

Decoder::~Decoder(){
    reset();
}

void Decoder::add(Accessory* item = nullptr){
    if (item != nullptr)
        _list.PushBack(item);
}

void Decoder::remove(uint8_t index){
    if (isValidObject(index))
        delete _list[index];
}

void Decoder::reset(){
    for (int i_ = 0; i_ < _list.Size(); ++i_) {
        remove(i_);
    }
    _list.Erase(0, _list.Size());
    _list.Clear();
    Vector<Accessory*>().Swap(_list);
}

void Decoder::notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower){
#ifdef  NOTIFY_TURNOUT_MSG
    Serial.print("notifyDccAccTurnoutOutput: Turnout: ");
    Serial.print(Addr, DEC);
    Serial.print(" Direction: ");
    Serial.print(Direction ? "Closed" : "Thrown");
    Serial.print(" Output: ");
    Serial.println(OutputPower ? "On" : "Off");
#endif

    for (int i_ = 0; i_ < _list.Size(); i_++) {
        _list[i_]->notifyTurnoutAddress(Addr, Direction, OutputPower);
    }
}

void Decoder::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps){
    for (int i = 0; i < _list.Size(); i++) {
        _list[i]->notifyDccSpeed(Addr, Speed, ForwardDir, SpeedSteps);
    }
}

void Decoder::notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State){
    uint8_t OutputNum_ = OutputAddr >> 1;  //shift over the bits so the outputaddr is 0 to 3
    uint8_t Cmd_ = OutputAddr & 0b00000001;  //JMRI puts out the state as the right most bit of pDccMsg->Data[1], the state argument doesnt change in JMRI Turnout.

#ifdef NOTIFY_DCC_ACCSTATE_MSG
    Serial.print("AccState - ");
    Serial.print("Raw addr: ");
    Serial.print(Addr);
    Serial.print(" BoardAddr: ");
    Serial.print(BoardAddr, DEC);
    Serial.print(" OutputAddr: ");
    Serial.print(OutputAddr, DEC);
    Serial.print(" Output: ");
    Serial.print(OutputNum_);
    Serial.print(" State: 0b");
    Serial.print(State, BIN);
    Serial.print(" Command: ");
    Serial.print(Cmd_ == 1 ? "Closed" : "Thrown");
    Serial.println();
    //Serial.print("    zDecoder: "); Serial.println(BoardAddr + OutputAddr);
#endif 

#ifdef  NOTIFY_TURNOUT_MSG
    Serial.print("Decoder: Address"); Serial.print(Addr);
    Serial.print(Cmd_ == 1 ? " :Closed" : " :Thrown");
    Serial.println();
#endif

    for (int i_ = 0; i_ < _list.Size(); i_++) {
        _list[i_]->notifyAddress(Addr, Cmd_);
    }

}

void Decoder::notifyDccFunc(uint16_t Addr, uint8_t FuncNum, uint8_t FuncState){
    if (FuncNum != 1)
        return;

    for (int i = 0; i < _list.Size(); i++) {

        if (FuncState & 0x10)
            _list[i]->on();
        else
            _list[i]->off();
    }
}

void Decoder::process() {
    for (uint8_t i_ = 0; i_ < _list.Size(); i_++)
        _list[i_]->process();
}

void Decoder::on(uint8_t index){
    if (isValidObject(index)) {
        _list[index]->on();
    }
}

void Decoder::off(uint8_t index){
    if (isValidObject(index)) {
        _list[index]->off();
    }
}

void Decoder::toggling(uint8_t index){
    if (isValidObject(index)) {
        if (_list[index]->isOn()) {
            _list[index]->off();
        }
        else {
            _list[index]->on();
        }
    }
}

bool Decoder::isOn(uint8_t index) {
    if (isValidObject(index)) {
        return _list[index]->isOn();
    }
    return false;
}

AccessoryType Decoder::getAccessoryType(uint8_t index){
    if (isValidObject(index)) {
        return _list[index]->getType();
    }
    return AccessoryType::None;
}

uint16_t Decoder::getAddress(uint8_t index){
    if (isValidObject(index))
        return _list[index]->myAddress();
    return 0;
}

uint8_t Decoder::getMode(uint8_t index){
    if (isValidObject(index))
        return _list[index]->myMode();
    return 0;
}

bool Decoder::isValidObject(uint8_t index) {
	return index < _list.Size() && _list[index] != nullptr;
}