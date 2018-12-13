/*-------------------------------- Arctic Core ------------------------------
 * Copyright (C) 2013, ArcCore AB, Sweden, www.arccore.com.
 * Contact: <contact@arccore.com>
 *
 * Copyright (C) 2018  AS <parai@foxmail.com>
 *
 * You may ONLY use this file:
 * 1)if you have a valid commercial ArcCore license and then in accordance with
 * the terms contained in the written license agreement between you and ArcCore,
 * or alternatively
 * 2)if you follow the terms found in GNU General Public License version 2 as
 * published by the Free Software Foundation and appearing in the file
 * LICENSE.GPL included in the packaging of this file or here
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *-------------------------------- Arctic Core -----------------------------*/

/** @reqSettings DEFAULT_SPECIFICATION_REVISION=4.2.2 */

#include "SD.h"
#include "SD_Internal.h"

#define CONFIG_TYPE        0x01
#define IPV4ENDPOINT_TYPE  0x04
#define IPV4MULTICAST_TYPE 0x14

char keypair_pool[500]; /* IMPROVEMENT use dynamic allocation for keys and pairs when it can be freed after use
                           or create exact sum length from generator  */
/* calculates a random max delay time for  Initial Wait phase */
uint32 RandomDelay(uint32 min_ms, uint32 max_ms)
{
    uint32 r;
    r = min_ms +  rand() / ( RAND_MAX / (max_ms-min_ms+1) +1);

    return r;
}

/* Fills the uint8 array with data from the Sd_Message
 * according to message format in chapter 7.3 of SD spec.
 *
 */
void FillMessage(Sd_Message* msg, uint8* message, uint32 *length)
{
    WRITE32_NA(&message[0], msg->MessageID);
    WRITE32_NA(&message[4], msg->Length);
    WRITE32_NA(&message[8], msg->RequestID);

    message[12] = msg->ProtocolVersion;
    message[13] = msg->InterfaceVersion;
    message[14] = msg->MessageType;
    message[15] = msg->ReturnCode;

    message[16] = msg->Flags;
    WRITE24_NA(&message[17], msg->Reserved);

    WRITE32_NA(&message[20], msg->LengthOfEntriesArray);

    /* Entries Array */
    memcpy(&message[24], msg->EntriesArray, msg->LengthOfEntriesArray);

    WRITE32_NA(&message[24+msg->LengthOfEntriesArray], msg->LengthOfOptionsArray);

    /* Options array */
    memcpy(&message[28+msg->LengthOfEntriesArray], msg->OptionsArray, msg->LengthOfOptionsArray);

    *length = 28+msg->LengthOfEntriesArray+msg->LengthOfOptionsArray;
}

/* Fills the uint8 array with data from the Sd_Type1Entry
 * according to message format in chapter 7.3 of SD spec.
 *
 */
/** @req 4.2.2/SWS_SD_00161 */
/** @req 4.2.2/SWS_SD_00159 **/
void FillType1Entry(Sd_Entry_Type1_Services entry, uint8 *entry_array){

    entry_array[0] = entry.Type;
    entry_array[1] = entry.IndexFirstOptionRun;
    entry_array[2] = entry.IndexSecondOptionRun;
    entry_array[3] = (((entry.NumberOfOption1 << 4) & 0xF0) | (entry.NumberOfOption2 & 0x0F));

    WRITE16_NA(&entry_array[4], entry.ServiceID);
    WRITE16_NA(&entry_array[6], entry.InstanceID);

    entry_array[8] = entry.MajorVersion;
    WRITE24_NA(&entry_array[9], entry.TTL);

    WRITE32_NA(&entry_array[12], entry.MinorVersion);
}

/* Fills the uint8 array with data from the Sd_Type2Entry
 * according to message format in chapter 7.3 of SD spec.
 *
 */
/** @req 4.2.2/SWS_SD_0184 */
void FillType2Entry(Sd_Entry_Type2_EventGroups entry, uint8 *entry_array){

    uint32 row = 0;

    entry_array[0] = entry.Type;
    entry_array[1] = entry.IndexFirstOptionRun;
    entry_array[2] = entry.IndexSecondOptionRun;
    entry_array[3] = (((entry.NumberOfOption1 << 4) & 0xF0) | (entry.NumberOfOption2 & 0x0F));
    WRITE16_NA(&entry_array[4], entry.ServiceID);
    WRITE16_NA(&entry_array[6], entry.InstanceID);
    entry_array[8] = entry.MajorVersion;
    WRITE24_NA(&entry_array[9], entry.TTL);
    row = (uint32) (((entry.Counter & 0x0F) << 16) | (entry.EventgroupID));
    WRITE32_NA(&entry_array[12], row);

}

void DecodeType1Entry(uint8 *entries_array, Sd_Entry_Type1_Services *entry)
{

    uint32 row;

    entry->Type = entries_array[0];
    entry->IndexFirstOptionRun = entries_array[1];
    entry->IndexSecondOptionRun = entries_array[2];
    entry->NumberOfOption1 = (uint8) ((entries_array[3] & 0xF0) >> 4);
    entry->NumberOfOption2 = (uint8) (entries_array[3] & 0xF);

    entry->ServiceID = entries_array[4] * 256 + entries_array[5];
    entry->InstanceID = entries_array[6] * 256 + entries_array[7];

    entry->MajorVersion = entries_array[8];
    entry->TTL = (uint32) READ24_NA(&entries_array[9]);

    entry->MinorVersion = READ32_NA(&entries_array[12]);;

}

void DecodeType2Entry(uint8 *entries_array, Sd_Entry_Type2_EventGroups *entry)
{
    uint32 row;

    entry->Type = entries_array[0];
    entry->IndexFirstOptionRun = entries_array[1];
    entry->IndexSecondOptionRun = entries_array[2];
    entry->NumberOfOption1 = (uint8) ((entries_array[3] & 0xF0) >> 4);
    entry->NumberOfOption2 = (uint8) (entries_array[3] & 0xF);

    entry->ServiceID = entries_array[4] * 256 + entries_array[5];
    entry->InstanceID = entries_array[6] * 256 + entries_array[7];

    entry->MajorVersion = entries_array[8];
    entry->TTL = (uint32) READ24_NA(&entries_array[9]);;

    row = READ32_NA(&entries_array[12]);
    entry->Counter = (uint8) ((row &0x000F0000) >> 16);
    entry->EventgroupID = (uint16) (row & 0x0000FFFF);

}

/** @req 4.2.2/SWS_SD_0479 **/
void DecodeMessage(Sd_Message *msg, uint8* message, uint32 length)
{
    msg->MessageID = READ32_NA(&message[0]);
    msg->Length    = READ32_NA(&message[4]);
    msg->RequestID = READ32_NA(&message[8]);

    msg->ProtocolVersion = message[12];
    msg->InterfaceVersion = message[13];
    msg->MessageType = message[14];
    msg->ReturnCode = message[15];

    msg->Flags = message[16];
    msg->Reserved = READ24_NA(&message[17]);

    msg->LengthOfEntriesArray = READ32_NA(&message[20]);

    msg->EntriesArray = (uint8 *)&message[24];

    const uint32 OptionsArrayOffset = (msg->LengthOfEntriesArray + 24);

    if (length > OptionsArrayOffset){
        msg->LengthOfOptionsArray = READ32_NA(&message[OptionsArrayOffset]);

        if (msg->LengthOfOptionsArray > 0) {
            msg->OptionsArray = (uint8 *)&message[OptionsArrayOffset + 4];
        }
        else {
            msg->OptionsArray = NULL;
        }
    }
    else
    {
        msg->LengthOfOptionsArray = 0;
        msg->OptionsArray = NULL;
    }
}

void DecodeOptionConfiguration (uint8 *options[], Sd_CapabilityRecordType capabilty_record[], uint8 *no_of_capabilty_records )
{
    uint8 index = 0;
    uint8 len = 0;
    uint8 offset = 4u;
    uint8 separator_pos = 0;
    uint8 i;
    uint8 pool_index = 0;

    for (uint8 opt=0;opt<MAX_OPTIONS;opt++){
        if (options[opt] != NULL){
            uint8 type = options[opt][2];
            if (type == CONFIG_TYPE){
                for(;options[opt][offset] != 0x00;){
                    len =  options[opt][offset];
                    offset = offset+1;
                    separator_pos = len;
                    for (i = 0; i < len; i++){
                        if( options[opt][offset+i] == '='){
                            separator_pos = i;
                            break;
                        }
                    }
                    capabilty_record[index].Key = &keypair_pool[pool_index];
                    pool_index += separator_pos+1;
                    memcpy(capabilty_record[index].Key, &options[opt][offset], separator_pos);
                    capabilty_record[index].Key[separator_pos] = '\0';

                    if((separator_pos != len) || ((len-separator_pos) != 1)){/* separator may not be there or value may not be there */
                        capabilty_record[index].Value = &keypair_pool[pool_index];
                        pool_index += len-separator_pos;
                        memcpy(capabilty_record[index].Value, &options[opt][offset+separator_pos+1], len-separator_pos-1);
                        capabilty_record[index].Value[len-separator_pos-1] = '\0';
                    }
                    index++;
                    if(options[opt][len+offset] == 0x00){
                        break;
                    }
                    offset += len;
                    len = 0;
                }
            }
        }
    }
    *no_of_capabilty_records = index;
}


void DecodeOptionIpv4Endpoint (uint8 *options[], Ipv4Endpoint endpoint[], uint8 *no_of_endpoints)
{
    uint8 index = 0;
    uint32 row = 0;
    uint16 value = 0;

    for (uint8 opt=0;opt<MAX_OPTIONS;opt++){
        if (options[opt] != NULL){
            uint8 type = options[opt][2];
            if (type == IPV4ENDPOINT_TYPE){
                endpoint[index].IPv4Address = READ32_NA(&options[opt][4]);
                endpoint[index].Protocol = options[opt] [9];
                endpoint[index].PortNumber = READ16_NA(&options[opt][10]);
                index++;
            }
        }
    }
    *no_of_endpoints = index;
}

void DecodeOptionIpv4Multicast (uint8 *options[], Ipv4Multicast multicast[], uint8 *no_of_multicasts)
{
    uint8 index = 0;
    uint32 row = 0;
    uint16 value = 0;

    for (uint8 opt=0;opt<MAX_OPTIONS;opt++){
        if (options[opt] != NULL){
            uint8 type = options[opt][2];
            if (type == IPV4MULTICAST_TYPE){
                multicast[index].IPv4Address = READ32_NA(&options[opt][4]);
                multicast[index].Protocol = options[opt] [9];
                multicast[index].PortNumber = READ16_NA(&options[opt][10]);
                index++;
            }
        }
    }
    *no_of_multicasts = index;
}
