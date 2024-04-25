#pragma once

#include <vector>

#define preference_started_before "run"
#define preference_config_version "confVersion"
#define preference_device_id_lock "deviceId"
#define preference_device_id_opener "deviceIdOp"
#define preference_nuki_id_lock "nukiId"
#define preference_nuki_id_opener "nukidOp"
#define preference_mqtt_broker "mqttbroker"
#define preference_mqtt_broker_port "mqttport"
#define preference_mqtt_user "mqttuser"
#define preference_mqtt_password "mqttpass"
#define preference_mqtt_log_enabled "mqttlog"
#define preference_lock_enabled "lockena"
#define preference_lock_pin_status "lockpin"
#define preference_mqtt_lock_path "mqttpath"
#define preference_opener_enabled "openerena"
#define preference_opener_pin_status "openerpin"
#define preference_opener_continuous_mode "openercont"
#define preference_mqtt_opener_path "mqttoppath"
#define preference_check_updates "checkupdates"
#define preference_lock_max_keypad_code_count "maxkpad"
#define preference_opener_max_keypad_code_count "opmaxkpad"
#define preference_mqtt_ca "mqttca"
#define preference_mqtt_crt "mqttcrt"
#define preference_mqtt_key "mqttkey"
#define preference_mqtt_hass_discovery "hassdiscovery"
#define preference_mqtt_hass_cu_url "hassConfigUrl"
#define preference_ip_dhcp_enabled "dhcpena"
#define preference_ip_address "ipaddr"
#define preference_ip_subnet "ipsub"
#define preference_ip_gateway "ipgtw"
#define preference_ip_dns_server "dnssrv"
#define preference_network_hardware "nwhw"
#define preference_network_hardware_gpio "nwhwdt" // obsolete
#define preference_network_wifi_fallback_disabled "nwwififb"
#define preference_rssi_publish_interval "rssipb"
#define preference_hostname "hostname"
#define preference_network_timeout "nettmout"
#define preference_restart_on_disconnect "restdisc"
#define preference_restart_timer "resttmr"
#define preference_restart_ble_beacon_lost "rstbcn"
#define preference_query_interval_lockstate "lockStInterval"
#define preference_query_interval_configuration "configInterval"
#define preference_query_interval_battery "batInterval"
#define preference_query_interval_keypad "kpInterval"
#define preference_access_level "accLvl"
#define preference_admin_enabled "aclConfig"
#define preference_keypad_info_enabled "kpInfoEnabled"
#define preference_keypad_control_enabled "kpCntrlEnabled"
#define preference_publish_authdata "pubAuth"
#define preference_acl "aclLckOpn"
#define preference_register_as_app "regAsApp" // true = register as hub; false = register as app
#define preference_command_nr_of_retries "nrRetry"
#define preference_command_retry_delay "rtryDelay"
#define preference_cred_user "crdusr"
#define preference_cred_password "crdpass"
#define preference_gpio_locking_enabled "gpiolck" // obsolete
#define preference_gpio_configuration "gpiocfg"
#define preference_publish_debug_info "pubdbg"
#define preference_presence_detection_timeout "prdtimeout"
#define preference_has_mac_saved "hasmac"
#define preference_has_mac_byte_0 "macb0"
#define preference_has_mac_byte_1 "macb1"
#define preference_has_mac_byte_2 "macb2"
#define preference_latest_version "latest"

class DebugPreferences
{
private:
    std::vector<char*> _keys =
    {
            preference_started_before, preference_config_version, preference_device_id_lock, preference_device_id_opener, preference_nuki_id_lock, preference_nuki_id_opener,  preference_mqtt_broker, preference_mqtt_broker_port, preference_mqtt_user, preference_mqtt_password, preference_mqtt_log_enabled, preference_check_updates,
            preference_lock_enabled, preference_lock_pin_status, preference_mqtt_lock_path, preference_opener_enabled, preference_opener_pin_status,
            preference_opener_continuous_mode, preference_mqtt_opener_path, preference_lock_max_keypad_code_count, preference_opener_max_keypad_code_count,
            preference_mqtt_ca, preference_mqtt_crt, preference_mqtt_key, preference_mqtt_hass_discovery, preference_mqtt_hass_cu_url,
            preference_ip_dhcp_enabled, preference_ip_address, preference_ip_subnet, preference_ip_gateway, preference_ip_dns_server,
            preference_network_hardware, preference_network_wifi_fallback_disabled, preference_rssi_publish_interval,
            preference_hostname, preference_network_timeout, preference_restart_on_disconnect,
            preference_restart_ble_beacon_lost, preference_query_interval_lockstate,
            preference_query_interval_configuration, preference_query_interval_battery, preference_query_interval_keypad,
            preference_keypad_control_enabled, preference_admin_enabled, preference_keypad_info_enabled, preference_acl,
            preference_access_level, preference_register_as_app, preference_command_nr_of_retries,
            preference_command_retry_delay, preference_cred_user, preference_cred_password, preference_publish_authdata,
            preference_publish_debug_info, preference_presence_detection_timeout,
            preference_has_mac_saved, preference_has_mac_byte_0, preference_has_mac_byte_1, preference_has_mac_byte_2, preference_latest_version,
    };
    std::vector<char*> _redact =
    {
        preference_mqtt_user, preference_mqtt_password,
        preference_mqtt_ca, preference_mqtt_crt, preference_mqtt_key,
        preference_cred_user, preference_cred_password,
        preference_nuki_id_lock, preference_nuki_id_opener,
    };
    std::vector<char*> _boolPrefs =
    {
            preference_started_before, preference_mqtt_log_enabled, preference_check_updates, preference_lock_enabled, preference_opener_enabled, preference_opener_continuous_mode,
            preference_restart_on_disconnect, preference_keypad_control_enabled, preference_admin_enabled, preference_keypad_info_enabled,
            preference_register_as_app, preference_ip_dhcp_enabled,
            preference_publish_authdata, preference_has_mac_saved, preference_publish_debug_info, preference_network_wifi_fallback_disabled
    };

    const bool isRedacted(const char* key) const
    {
        return std::find(_redact.begin(), _redact.end(), key) != _redact.end();
    }
    const String redact(const String s) const
    {
        return s == "" ? "" : "***";
    }
    const String redact(const int32_t i) const
    {
        return i == 0 ? "" : "***";
    }
    const String redact(const uint32_t i) const
    {
        return i == 0 ? "" : "***";
    }
    const String redact(const int64_t i) const
    {
        return i == 0 ? "" : "***";
    }
    const String redact(const uint64_t i) const
    {
        return i == 0 ? "" : "***";
    }

    const void appendPreferenceInt8(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const int32_t)preferences->getChar(key)) : String(preferences->getChar(key)));
        s.concat("\n");
    }
    const void appendPreferenceUInt8(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const uint32_t)preferences->getUChar(key)) : String(preferences->getUChar(key)));
        s.concat("\n");
    }
    const void appendPreferenceInt16(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const int32_t)preferences->getShort(key)) : String(preferences->getShort(key)));
        s.concat("\n");
    }
    const void appendPreferenceUInt16(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const uint32_t)preferences->getUShort(key)) : String(preferences->getUShort(key)));
        s.concat("\n");
    }
    const void appendPreferenceInt32(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const int32_t)preferences->getInt(key)) : String(preferences->getInt(key)));
        s.concat("\n");
    }
    const void appendPreferenceUInt32(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const uint32_t)preferences->getUInt(key)) : String(preferences->getUInt(key)));
        s.concat("\n");
    }
    const void appendPreferenceInt64(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const int64_t)preferences->getLong64(key)) : String(preferences->getLong64(key)));
        s.concat("\n");
    }
    const void appendPreferenceUInt64(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const uint64_t)preferences->getULong64(key)) : String(preferences->getULong64(key)));
        s.concat("\n");
    }
    const void appendPreferenceBool(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(preferences->getBool(key) ? "true" : "false");
        s.concat("\n");
    }
    const void appendPreferenceString(Preferences *preferences, String& s, const char* description, const char* key)
    {
        s.concat(description);
        s.concat(": ");
        s.concat(isRedacted(key) ? redact((const String)preferences->getString(key)) : preferences->getString(key));
        s.concat("\n");
    }

    const void appendPreference(Preferences *preferences, String& s, const char* key)
    {
        if(std::find(_boolPrefs.begin(), _boolPrefs.end(), key) != _boolPrefs.end())
        {
            appendPreferenceBool(preferences, s, key, key);
            return;
        }

        switch(preferences->getType(key))
        {
            case PT_I8:
                appendPreferenceInt8(preferences, s, key, key);
                break;
            case PT_I16:
                appendPreferenceInt16(preferences, s, key, key);
                break;
            case PT_I32:
                appendPreferenceInt32(preferences, s, key, key);
                break;
            case PT_I64:
                appendPreferenceInt64(preferences, s, key, key);
                break;
            case PT_U8:
                appendPreferenceUInt8(preferences, s, key, key);
                break;
            case PT_U16:
                appendPreferenceUInt16(preferences, s, key, key);
                break;
            case PT_U32:
                appendPreferenceUInt32(preferences, s, key, key);
                break;
            case PT_U64:
                appendPreferenceUInt64(preferences, s, key, key);
                break;
            case PT_STR:
                appendPreferenceString(preferences, s, key, key);
                break;
            default:
                appendPreferenceString(preferences, s, key, key);
                break;
        }
    }

public:
    const String preferencesToString(Preferences *preferences)
    {
        String s = "";

        for(const auto& key : _keys)
        {
            appendPreference(preferences, s, key);
        }

        return s;
    }

};