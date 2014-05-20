/*
 * cc3000_wlan.c
 *
 *  Created on: 05.09.2013
 *      Author: Johannes
 */

#include "cc3000_wlan.h"
#include "cc3000_platform.h"
#include "cc3000_event_handler.h"
#include "cc3000_hci.h"
#include "cc3000_spi.h"

#include <string.h>

//*****************************************************************************
//
//! cc3000_wlan_disconnect
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	send command to disconnect from the WLAN
//!
//
//*****************************************************************************
void cc3000_wlan_disconnect(void) {

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_disconnect\n");
#endif
	// increase the desired state
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_WLAN_DISCONNECT, 0);
	cc3000_hci_end_command();
	// tell the event handler what command we expect back
	cc3000_hci_expect(HCI_EVNT_WLAN_DISCONNECT);

}
//*****************************************************************************
//
//! cc3000_wlan_get_status
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	send command to get current WLAN status
//!
//
//*****************************************************************************
void cc3000_wlan_get_status(void) {

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_get_status\n");
#endif
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_STATUSGET, 0);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_STATUSGET);

}
//*****************************************************************************
//
//! cc3000_wlan_set_scan_params
//!
//!  @param  	scan_frequency
//!
//!  @param  	channel_mask
//!
//!  @return  	none
//!
//!  @brief  	send command to set the scan parameter
//!
//
//*****************************************************************************
void cc3000_wlan_set_scan_params(uint32 scan_frequency, uint16 channel_mask ) {

	uint8 count;

	#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_set_scan_params\n");
#endif
	cc3000_desired_state++;

	cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_SET_SCANPARAM, 25*4);
	cc3000_hci_send_uint32(36);	// This 36 appears to be undocumented and magic
	cc3000_hci_send_uint32(scan_frequency);
	cc3000_hci_send_uint32(100);	// min dwell time
	cc3000_hci_send_uint32(100);	// max dwell time
	cc3000_hci_send_uint32(5);	// max probe request between dwell time
	cc3000_hci_send_uint32(channel_mask);
	cc3000_hci_send_uint32((uint32)-80);	// rssi threshold
	cc3000_hci_send_uint32(0);	// SNR threshold
	cc3000_hci_send_uint32(205);	// probe tx power
	for (count = 0; count < 16; count++) {
		cc3000_hci_send_uint32(2000);	// each channel entry periodic scan
	}
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_SET_SCANPARAM);
}
//*****************************************************************************
//
//! cc3000_wlan_connect
//!
//!  @param  	ssid		ssid of the ap to connect to
//!
//!  @param  	sec_type	secutiry type of the ap (can be WLAN_SEC_UNSEC,
//!							WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2)
//!
//!  @param  	key			ap key
//!
//!  @return  	none
//!
//!  @brief  	send command to connect to a given ap
//!
//
//*****************************************************************************
void cc3000_wlan_connect(uint8 *ssid, uint8 sec_type, uint8 *key) {

	uint8 key_len;
	uint8 ssid_len;
	uint8 count;

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
debug_str("cc3000_wlan_connect\n");
#endif
	cc3000_desired_state++;

	ssid_len = strlen(ssid);
	key_len  = strlen(key);

	cc3000_hci_start_command(HCI_CMND_WLAN_CONNECT, 28 + ssid_len + key_len);
	cc3000_hci_send_uint32(0x0000001c);	// magic
	cc3000_hci_send_uint32(ssid_len);
	cc3000_hci_send_uint32(sec_type);
	cc3000_hci_send_uint32(0x10 + ssid_len);
	cc3000_hci_send_uint32(key_len);
	cc3000_hci_send_uint16(0);	// magic 0x0000

	// bssid
	for (count = 0; count < 6; count++) {
		//*(ptr)++ = 0;
		cc3000_hci_send_uint8(0);
	}

	// ssid
	for (count = 0; count < ssid_len; count++) {
		//*(ptr)++ = ssid[count];
		cc3000_hci_send_uint8(ssid[count]);
	}

	// key

	if ((key) && (key_len)) {
		for (count = 0; count < key_len; count++) {
			//*(ptr)++ = key[count];
			cc3000_hci_send_uint8(key[count]);
		}
	}
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_WLAN_CONNECT);
}
//*****************************************************************************
//
//! cc3000_wlan_set_event_mask
//!
//!  @param  	mask	events to mask
//!
//!  @return  	none
//!
//!  @brief  	send command to set the event mask for unsolicited events
//!
//
//*****************************************************************************
void cc3000_wlan_set_event_mask(uint16 mask) {

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
debug_str("cc3000_wlan_set_event_mask\n");
#endif
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_EVENT_MASK, 4);
	cc3000_hci_send_uint32(mask);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_EVENT_MASK);

}
//*****************************************************************************
//
//! cc3000_wlan_set_connection_policy
//!
//!  @param  	fast_connect	use fast_connect
//!
//!  @param  	open_ap_connect	connect to any open ap
//!
//!  @param  	use profiles	use saved profiles to connect
//!
//!  @return  	none
//!
//!  @brief  	send command to set the connection policy
//!
//
//*****************************************************************************
void cc3000_wlan_set_connection_policy(enum fast_connect_options_enum fast_connect,
											   enum open_ap_connect_options_enum open_ap_connect,
 											   enum use_profiles_options_enum use_profiles) {

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
debug_str("cc3000_wlan_set_event_mask\n");
#endif
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY, 3*4);
	cc3000_hci_send_uint32(open_ap_connect);
	cc3000_hci_send_uint32(fast_connect);
	cc3000_hci_send_uint32(use_profiles);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY);

}

//*****************************************************************************
//
//! cc3000_wlan_req_scan_result
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	send command to request the WLAN scan results
//!
//
//*****************************************************************************
void cc3000_wlan_req_scan_result(void){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_req_scan_result\n");
#endif
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS, 4);
	cc3000_hci_send_uint32(0x0000);	// magic
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS);
}
//*****************************************************************************
//
//! cc3000_wlan_get_scan_result
//!
//!  @param  	ap_entry	pointer to AP entry list
//!
//!  @return  	none
//!
//!  @brief  	saves and converts the received data stream to an AP entry
//!
//
//*****************************************************************************
void cc3000_wlan_get_scan_result(ap_entry_t *ap_entry) {

	uint8 ssid_length;
	uint8 count;
	// ToDO: determine sane buffer size
	uint8 wlan_data[50];

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_req_scan_result\n");
#endif

	count = get_return_size();
	cc3000_get_return_stream(wlan_data,0,count);

	ap_entry->entry_count = wlan_data[0];
	ap_entry->scan_status = wlan_data[4];

	if (wlan_data[8] & 0b10000000) {
		ap_entry->entry_valid = 1;
	} else {
		ap_entry->entry_valid = 0;
	}

	ap_entry->entry_valid = wlan_data[8] >> 6;
	ap_entry->rssi = wlan_data[8] & 0b01111111;
	ap_entry->security = wlan_data[9] >> 6;
	ssid_length = wlan_data[9] & 0b00111111;

	for (count = 0; count < ssid_length; count++) {
		ap_entry->ssid[count] = wlan_data[12 + count];
	}
	ap_entry->ssid[count + 1] = '\0';

}

//*****************************************************************************
//
//!  wlan_add_profile
//!
//!  @param    ulSecType  WLAN_SEC_UNSEC,WLAN_SEC_WEP,WLAN_SEC_WPA,WLAN_SEC_WPA2
//!  @param    ucSsid    ssid  SSID up to 32 bytes
//!  @param    ulSsidLen ssid length
//!  @param    ucBssid   bssid  6 bytes
//!  @param    ulPriority ulPriority profile priority. Lowest priority:0.
//!  @param    ulPairwiseCipher_Or_TxKeyLen  key length for WEP security
//!  @param    ulGroupCipher_TxKeyIndex  key index
//!  @param    ulKeyMgmt        KEY management
//!  @param    ucPf_OrKey       security key
//!  @param    ulPassPhraseLen  security key length for WPA\WPA2
//!
//!  @return    On success, zero is returned. On error, -1 is returned
//!
//!  @brief     When auto start is enabled, the device connects to
//!             station from the profiles table. Up to 7 profiles are supported.
//!             If several profiles configured the device choose the highest
//!             priority profile, within each priority group, device will choose
//!             profile based on security policy, signal strength, etc
//!             parameters. All the profiles are stored in CC3000 NVMEM.
//!
//!  @sa        wlan_ioctl_del_profile
//
//*****************************************************************************

void cc3000_wlan_add_profile(uint32 ulSecType,
								 uint8 *ucSsid,
								 uint32 ulSsidLen,
								 uint8 *ucBssid,
								 uint32 ulPriority,
								 uint32 ulPairwiseCipher_Or_TxKeyLen,
								 uint32 ulGroupCipher_TxKeyIndex,
								 uint32 ulKeyMgmt,
								 uint8 *ucPf_OrKey,
								 uint32 ulPassPhraseLen){
	uint8 arg_len;
	uint8 *p;
	int32 i = 0;
	uint8 bssid_zero[] = {0, 0, 0, 0, 0, 0};

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_add_profile\n");
#endif

	cc3000_desired_state++;

	// Setup arguments in accordance with the security type
	switch (ulSecType){
		//OPEN
	case WLAN_SEC_UNSEC:{
			arg_len = 14 + ETH_ALEN + ulSsidLen;
			cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_ADD_PROFILE, arg_len);
			cc3000_hci_send_uint32(0x00000014);
			cc3000_hci_send_uint32(ulSsidLen);
			cc3000_hci_send_uint16(0);
			if(ucBssid){
				cc3000_hci_send_stream(ucBssid, ETH_ALEN);
			}else{
				cc3000_hci_send_stream(bssid_zero, ETH_ALEN);
			}
			cc3000_hci_send_uint32(ulPriority);
			cc3000_hci_send_stream(ucSsid, ulSsidLen);
		}
		break;

		//WEP
	case WLAN_SEC_WEP:{
			arg_len = 28 + ETH_ALEN + ulSsidLen + (4 * ulPairwiseCipher_Or_TxKeyLen);
			cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_ADD_PROFILE, arg_len);
			cc3000_hci_send_uint32(0x00000020);
			cc3000_hci_send_uint32(ulSsidLen);
			cc3000_hci_send_uint32(0);
			if(ucBssid){
				cc3000_hci_send_stream(ucBssid, ETH_ALEN);
			}else{
				cc3000_hci_send_stream(bssid_zero, ETH_ALEN);
			}
			cc3000_hci_send_uint32(ulPriority);
			cc3000_hci_send_uint32(0x0000000C + ulSsidLen);
			cc3000_hci_send_uint32(ulPairwiseCipher_Or_TxKeyLen);
			cc3000_hci_send_uint32(ulGroupCipher_TxKeyIndex);
			cc3000_hci_send_stream(ucSsid, ulSsidLen);

			for(i = 0; i < 4; i++){
				p = &ucPf_OrKey[i * ulPairwiseCipher_Or_TxKeyLen];
				cc3000_hci_send_stream(p, ulPairwiseCipher_Or_TxKeyLen);
			}
		}
		break;

		//WPA
		//WPA2
	case WLAN_SEC_WPA:
	case WLAN_SEC_WPA2:{
			arg_len = 34 + ETH_ALEN + ulSsidLen + ulPassPhraseLen;
			cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_ADD_PROFILE, arg_len);
			cc3000_hci_send_uint32(0x00000028);
			cc3000_hci_send_uint32(ulSsidLen);
			cc3000_hci_send_uint16(0);
			if(ucBssid){
				cc3000_hci_send_stream(ucBssid, ETH_ALEN);
			}else{
				cc3000_hci_send_stream(bssid_zero, ETH_ALEN);
			}
			cc3000_hci_send_uint32(ulPriority);
			cc3000_hci_send_uint32(ulPairwiseCipher_Or_TxKeyLen);
			cc3000_hci_send_uint32(ulGroupCipher_TxKeyIndex);
			cc3000_hci_send_uint32(ulKeyMgmt);
			cc3000_hci_send_uint32(0x00000008 + ulSsidLen);
			cc3000_hci_send_uint32(ulPassPhraseLen);
			cc3000_hci_send_stream(ucSsid, ulSsidLen);
			cc3000_hci_send_stream(ucPf_OrKey, ulPassPhraseLen);
		}

		break;
	}
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_ADD_PROFILE);
}
//*****************************************************************************
//
//!  wlan_ioctl_del_profile
//!
//!  @param    index   number of profile to delete
//!
//!  @return    On success, zero is returned. On error, -1 is returned
//!
//!  @brief     Delete WLAN profile
//!
//!  @Note      In order to delete all stored profile, set index to 255.
//!
//!  @sa        wlan_add_profile
//
//*****************************************************************************
void cc3000_wlan_del_profile(uint32 ulIndex){

#if (CC3000_TRACE_LEVEL & CC3000_TRACE_LEVEL_DEBUG)
	debug_str("cc3000_wlan_del_profile\n");
#endif

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_WLAN_IOCTL_DEL_PROFILE, 4);
	cc3000_hci_send_uint32(ulIndex);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_CMND_WLAN_IOCTL_DEL_PROFILE);
}
