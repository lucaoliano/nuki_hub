#include <esp32-hal.h>
#include "Gpio.h"
#include "Config.h"
#include "Arduino.h"
#include "Logger.h"
#include "PreferencesKeys.h"
#include "RestartReason.h"
#include "../lib/gpio2go/src/Gpio2Go.h"

Gpio* Gpio::_inst = nullptr;
unsigned long Gpio::_debounceTs = 0;
const uint Gpio::_debounceTime = GPIO_DEBOUNCE_TIME;

Gpio::Gpio(Preferences* preferences)
: _preferences(preferences)
{
    _inst = this;
    loadPinConfiguration();

    if(_preferences->getBool(preference_gpio_locking_enabled))
    {
        migrateObsoleteSetting();
    }

    _inst->init();
}

void Gpio::init()
{
    for(const auto& entry : _inst->_pinConfiguration)
    {
        const auto it = std::find(_inst->availablePins().begin(), _inst->availablePins().end(), entry.pin);

        if(it == _inst->availablePins().end())
        {
            continue;
        }

        switch(entry.role)
        {
            case PinRole::InputLock:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrLock, FALLING);
                break;
            case PinRole::InputUnlock:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrUnlock, FALLING);
                break;
            case PinRole::InputUnlatch:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrUnlatch, FALLING);
                break;
            case PinRole::InputLockNgo:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrLockNgo, FALLING);
                break;
            case PinRole::InputLockNgoUnlatch:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrLockNgoUnlatch, FALLING);
                break;
            case PinRole::InputElectricStrikeActuation:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrElectricStrikeActuation, FALLING);
                break;
            case PinRole::InputActivateRTO:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrActivateRTO, FALLING);
                break;
            case PinRole::InputActivateCM:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrActivateCM, FALLING);
                break;
            case PinRole::InputDeactivateRtoCm:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrDeactivateRtoCm, FALLING);
                break;
            case PinRole::InputDeactivateRTO:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrDeactivateRTO, FALLING);
                break;
            case PinRole::InputDeactivateCM:
                pinMode(entry.pin, INPUT_PULLUP);
                attachInterrupt(entry.pin, isrDeactivateCM, FALLING);
                break;
            case PinRole::OutputHighLocked:
            case PinRole::OutputHighUnlocked:
            case PinRole::OutputHighMotorBlocked:
            case PinRole::OutputHighRtoActive:
            case PinRole::OutputHighCmActive:
            case PinRole::OutputHighRtoOrCmActive:
            case PinRole::GeneralOutput:
                pinMode(entry.pin, OUTPUT);
                break;
            case PinRole::GeneralInputPullDown:
                Gpio2Go::configurePin(entry.pin, PinMode::InputPullDown, InterruptMode::Change, 300);
                break;
            case PinRole::GeneralInputPullUp:
                Gpio2Go::configurePin(entry.pin, PinMode::InputPullup, InterruptMode::Change, 300);
                break;
            default:
                pinMode(entry.pin, OUTPUT);
                break;
        }

        Gpio2Go::subscribe(Gpio::inputCallback);
    }
}

const std::vector<uint8_t>& Gpio::availablePins() const
{
    return _availablePins;
}

void Gpio::loadPinConfiguration()
{
    size_t storedLength = _preferences->getBytesLength(preference_gpio_configuration);
    if(storedLength == 0)
    {
        return;
    }

    uint8_t serialized[storedLength];
    memset(serialized, 0, sizeof(serialized));

    size_t size = _preferences->getBytes(preference_gpio_configuration, serialized, sizeof(serialized));

    if(size == 0)
    {
        return;
    }

    size_t numEntries = size / 2;

    _pinConfiguration.clear();
    _pinConfiguration.reserve(numEntries);

    for(int i=0; i < numEntries; i++)
    {
        PinEntry entry;
        entry.pin = serialized[i * 2];
        entry.role = (PinRole) serialized[(i * 2 + 1)];
        if(entry.role != PinRole::Disabled)
        {
            _pinConfiguration.push_back(entry);
        }
    }
}

void Gpio::savePinConfiguration(const std::vector<PinEntry> &pinConfiguration)
{
    int8_t serialized[std::max(pinConfiguration.size() * 2, _preferences->getBytesLength(preference_gpio_configuration))];
    memset(serialized, 0, sizeof(serialized));

    int len = pinConfiguration.size();
    for(int i=0; i < len; i++)
    {
        const auto& entry = pinConfiguration[i];

        if(entry.role != PinRole::Disabled)
        {
            serialized[i * 2] = entry.pin;
            serialized[i * 2 + 1] = (int8_t) entry.role;
        }
    }

    _preferences->putBytes(preference_gpio_configuration, serialized, sizeof(serialized));
}

const std::vector<PinEntry> &Gpio::pinConfiguration() const
{
    return _pinConfiguration;
}

const PinRole Gpio::getPinRole(const int &pin) const
{
    for(const auto& pinEntry : _pinConfiguration)
    {
        if(pinEntry.pin == pin)
        {
            return pinEntry.role;
        }
    }
    return PinRole::Disabled;
}

String Gpio::getRoleDescription(PinRole role) const
{
    switch(role)
    {
        case PinRole::Disabled:
            return "Disabled";
        case PinRole::InputLock:
            return "Input: Lock";
        case PinRole::InputUnlock:
            return "Input: Unlock";
        case PinRole::InputUnlatch:
            return "Input: Unlatch";
        case PinRole::InputLockNgo:
            return "Input: Lock n Go";
        case PinRole::InputLockNgoUnlatch:
            return "Input: Lock n Go and unlatch";
        case PinRole::InputElectricStrikeActuation:
            return "Input: Electric strike actuation";
        case PinRole::InputActivateRTO:
            return "Input: Activate RTO";
        case PinRole::InputActivateCM:
            return "Input: Activate CM";
        case PinRole::InputDeactivateRtoCm:
            return "Input: Deactivate RTO/CM";
        case PinRole::InputDeactivateRTO:
            return "Input: Deactivate RTO";
        case PinRole::InputDeactivateCM:
            return "Input: Deactivate CM";
        case PinRole::OutputHighLocked:
            return "Output: High when locked";
        case PinRole::OutputHighUnlocked:
            return "Output: High when unlocked";
        case PinRole::OutputHighMotorBlocked:
            return "Output: High when motor blocked";
        case PinRole::OutputHighRtoActive:
            return "Output: High when RTO active";
        case PinRole::OutputHighCmActive:
            return "Output: High when CM active";
        case PinRole::OutputHighRtoOrCmActive:
            return "Output: High when RTO or CM active";
        case PinRole::GeneralOutput:
            return "General output";
        case PinRole::GeneralInputPullDown:
            return "General input (Pull-down)";
        case PinRole::GeneralInputPullUp:
            return "General input (Pull-up)";
        default:
            return "Unknown";
    }
}

void Gpio::getConfigurationText(String& text, const std::vector<PinEntry>& pinConfiguration, const String& linebreak) const
{
    for(const auto& entry : pinConfiguration)
    {
        if(entry.role != PinRole::Disabled)
        {
            text.concat("GPIO ");
            text.concat(entry.pin);
            if(entry.pin < 10)
            {
                text.concat(' ');
            }
            text.concat(": ");
            text.concat(getRoleDescription(entry.role));
            text.concat(linebreak);
        }
    }
}

const std::vector<PinRole>& Gpio::getAllRoles() const
{
    return _allRoles;
}

void Gpio::notify(const GpioAction &action, const int& pin)
{
    for(auto& callback : _callbacks)
    {
        callback(action, pin);
    }
}

void Gpio::inputCallback(const int &pin)
{
    _inst->notify(GpioAction::GeneralInput, pin);
}

void Gpio::addCallback(std::function<void(const GpioAction&, const int&)> callback)
{
    _callbacks.push_back(callback);
}

void Gpio::isrLock()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::Lock, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrUnlock()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::Unlock, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrUnlatch()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::Unlatch, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrLockNgo()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::LockNgo, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrLockNgoUnlatch()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::LockNgoUnlatch, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrElectricStrikeActuation()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::ElectricStrikeActuation, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrActivateRTO()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::ActivateRTO, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrActivateCM()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::ActivateCM, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrDeactivateRtoCm()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::DeactivateRtoCm, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrDeactivateRTO()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::DeactivateRTO, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::isrDeactivateCM()
{
    if(millis() < _debounceTs) return;
    _inst->notify(GpioAction::DeactivateCM, -1);
    _debounceTs = millis() + _debounceTime;
}

void Gpio::setPinOutput(const uint8_t& pin, const uint8_t& state)
{
    digitalWrite(pin, state);
}

void Gpio::migrateObsoleteSetting()
{
    _pinConfiguration.clear();

    PinEntry entry1;
    entry1.pin = 27;
    entry1.role = PinRole::InputUnlatch;

    PinEntry entry2;
    entry2.pin = 32;
    entry2.role = PinRole::InputLock;

    PinEntry entry3;
    entry3.pin = 33;
    entry3.role = PinRole::InputUnlock;

    _pinConfiguration.push_back(entry1);
    _pinConfiguration.push_back(entry2);
    _pinConfiguration.push_back(entry3);

    savePinConfiguration(_pinConfiguration);

    _preferences->remove(preference_gpio_locking_enabled);
    Log->println("Migrated gpio control setting");
    delay(200);
    restartEsp(RestartReason::GpioConfigurationUpdated);
}

