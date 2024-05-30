/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	EdpKit.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-09-13
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		EDPЭ��
	*
	*	�޸ļ�¼��	V1.1����strncpy�滻Ϊmemcpy�����Ǳ��bug
	************************************************************
	************************************************************
	************************************************************
**/

//Э��ͷ�ļ�

#include "edpkit.h"

//C��
#include <string.h>
#include "driver.h"

//==========================================================
//	�������ƣ�	EDP_NewBuffer
//
//	�������ܣ�	�����ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//				size����С
//
//	���ز�����	��
//
//	˵����		1.��ʹ�ö�̬�����������ڴ�
//				2.��ʹ�þֲ���ȫ��������ָ���ڴ�
//==========================================================
void EDP_NewBuffer(EDP_PACKET_STRUCTURE *edpPacket, uint32 size)
{
	
	uint32 i = 0;

	if(edpPacket->_data == NULL)
	{
		edpPacket->_memFlag = MEM_FLAG_ALLOC;
		
		edpPacket->_data = (uint8 *)EDP_MallocBuffer(size);
		if(edpPacket->_data != NULL)
		{
			edpPacket->_len = 0;
			
			edpPacket->_size = size;

			for(; i < edpPacket->_size; i++)
				edpPacket->_data[i] = 0;
		}
	}
	else
	{
		edpPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < edpPacket->_size; i++)
			edpPacket->_data[i] = 0;
		
		edpPacket->_len = 0;
		
		if(edpPacket->_size < size)
			edpPacket->_data = NULL;
	}

}

//==========================================================
//	�������ƣ�	EDP_DeleteBuffer
//
//	�������ܣ�	�ͷ������ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//
//	���ز�����	��
//
//	˵����		��ʹ�õľֲ���ȫ������ʱ���ͷ��ڴ�
//==========================================================
void EDP_DeleteBuffer(EDP_PACKET_STRUCTURE *edpPacket)
{

	if(edpPacket->_memFlag == MEM_FLAG_ALLOC)
		EDP_FreeBuffer(edpPacket->_data);
	
	edpPacket->_data = NULL;
	edpPacket->_len = 0;
	edpPacket->_size = 0;
	edpPacket->_memFlag = MEM_FLAG_NULL;

}

//==========================================================
//	�������ƣ�	EDP_UnPacketRecv
//
//	�������ܣ�	EDP���ݽ��������ж�
//
//	��ڲ�����	dataPtr�����յ�����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��ԭ��
//
//	˵����		
//==========================================================
uint8 EDP_UnPacketRecv(uint8 *dataPtr)
{
	
	return dataPtr[0];

}

//==========================================================
//	�������ƣ�	EDP_PacketConnect1
//
//	�������ܣ�	��¼��ʽ1���
//
//	��ڲ�����	devid���豸ID
//				apikey��APIKEY
//				cTime�����ӱ���ʱ��
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 EDP_PacketConnect1(const int8 *devid, const int8 *apikey, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint8 devid_len = strlen(devid);
	uint8 apikey_len = strlen(apikey);

	//�����ڴ�---------------------------------------------------------------------
	EDP_NewBuffer(edpPacket, 100);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0����������--------------------------------------------------------------
	edpPacket->_data[0] = CONNREQ;
	edpPacket->_len++;
	
	//Byte1��ʣ����Ϣ����----------------------------------------------------------
	edpPacket->_data[1] = 13 + devid_len + apikey_len;
	edpPacket->_len++;
	
	//Byte2~3��Э��������----------------------------------------------------------
	edpPacket->_data[2] = 0;
	edpPacket->_data[3] = 3;
	edpPacket->_len += 2;
	
	//Byte4~6��Э����--------------------------------------------------------------
	strncat((int8 *)edpPacket->_data + 4, "EDP", 3);
	edpPacket->_len += 3;
	
	//Byte7��Э��汾--------------------------------------------------------------
	edpPacket->_data[7] = 1;
	edpPacket->_len++;
	
	//Byte8�����ӱ�־--------------------------------------------------------------
	edpPacket->_data[8] = 0x40;
	edpPacket->_len++;
	
	//Byte9~10�����ӱ���ʱ��-------------------------------------------------------
	edpPacket->_data[9] = MOSQ_MSB(cTime);
	edpPacket->_data[10] = MOSQ_LSB(cTime);
	edpPacket->_len += 2;
	
	//Byte11~12��DEVID����---------------------------------------------------------
	edpPacket->_data[11] = MOSQ_MSB(devid_len);
	edpPacket->_data[12] = MOSQ_LSB(devid_len);
	edpPacket->_len += 2;
	
	//Byte13~13+devid_len��DEVID---------------------------------------------------
	strncat((int8 *)edpPacket->_data + 13, devid, devid_len);
	edpPacket->_len += devid_len;
	
	//Byte13+devid_len~13+devid_len+2��APIKEY����----------------------------------
	edpPacket->_data[13 + devid_len] = MOSQ_MSB(apikey_len);
	edpPacket->_data[14 + devid_len] = MOSQ_LSB(apikey_len);
	edpPacket->_len += 2;
	
	//Byte15+devid_len~15+devid_len+apikey_len��APIKEY-----------------------------
	strncat((int8 *)edpPacket->_data + 15 + devid_len, apikey, apikey_len);
	edpPacket->_len += apikey_len;
	

	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_PacketConnect2
//
//	�������ܣ�	��¼��ʽ2���
//
//	��ڲ�����	devid���豸ID
//				auth_key����Ȩ��Ϣ
//				cTime�����ӱ���ʱ��
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 EDP_PacketConnect2(const int8 *proid, const int8 *auth_key, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint8 proid_len = strlen(proid);
	uint8 authkey_len = strlen(auth_key);

	//�����ڴ�---------------------------------------------------------------------
	EDP_NewBuffer(edpPacket, 56);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0����������--------------------------------------------------------------
	edpPacket->_data[0] = CONNREQ;
	edpPacket->_len++;
	
	//Byte1��ʣ����Ϣ����----------------------------------------------------------
	edpPacket->_data[1] = 15 + proid_len + authkey_len;
	edpPacket->_len++;
	
	//Byte2~3��Э��������----------------------------------------------------------
	edpPacket->_data[2] = 0;
	edpPacket->_data[3] = 3;
	edpPacket->_len += 2;
	
	//Byte4~6��Э����--------------------------------------------------------------
	strncat((int8 *)edpPacket->_data + 4, "EDP", 3);
	edpPacket->_len += 3;
	
	//Byte7��Э��汾--------------------------------------------------------------
	edpPacket->_data[7] = 1;
	edpPacket->_len++;
	
	//Byte8�����ӱ�־--------------------------------------------------------------
	edpPacket->_data[8] = 0xC0;
	edpPacket->_len++;
	
	//Byte9~10�����ӱ���ʱ��-------------------------------------------------------
	edpPacket->_data[9] = MOSQ_MSB(cTime);
	edpPacket->_data[10] = MOSQ_LSB(cTime);
	edpPacket->_len += 2;
	
	//Byte11~12��DEVID����---------------------------------------------------------
	edpPacket->_data[11] = 0;
	edpPacket->_data[12] = 0;
	edpPacket->_len += 2;
	
	//Byte13~14��PROID����---------------------------------------------------------
	edpPacket->_data[13] = MOSQ_MSB(proid_len);
	edpPacket->_data[14] = MOSQ_LSB(proid_len);
	edpPacket->_len += 2;
	
	//Byte15~15+proid_len��RPOID---------------------------------------------------
	strncat((int8 *)edpPacket->_data + 15, proid, proid_len);
	edpPacket->_len += proid_len;
	
	//Byte15+devid_len~15+proid_len+1��APIKEY����----------------------------------
	edpPacket->_data[15 + proid_len] = MOSQ_MSB(authkey_len);
	edpPacket->_data[16 + proid_len] = MOSQ_LSB(authkey_len);
	edpPacket->_len += 2;
	
	//Byte17+proid_len~17+proid_len+apikey_len��APIKEY-----------------------------
	strncat((int8 *)edpPacket->_data + 17 + proid_len, auth_key, authkey_len);
	edpPacket->_len += authkey_len;
	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_UnPacketConnectRsp
//
//	�������ܣ�	���ӻظ����
//
//	��ڲ�����	rev_data�����յ�������
//
//	���ز�����	��¼���
//
//	˵����		
//==========================================================
uint8 EDP_UnPacketConnectRsp(uint8 *rev_data)
{

	//0		���ӳɹ�
	//1		��֤ʧ�ܣ�Э�����
	//2		��֤ʧ�ܣ��豸ID��Ȩʧ��
	//3		��֤ʧ�ܣ�������ʧ��
	//4		��֤ʧ�ܣ��û�ID��Ȩʧ��
	//5		��֤ʧ�ܣ�δ��Ȩ
	//6		��֤ʧ�ܣ���Ȩ����Ч
	//7		��֤ʧ�ܣ�������δ����
	//8		��֤ʧ�ܣ����豸�ѱ�����
	//9		��֤ʧ�ܣ��ظ��������������
	return rev_data[3];

}

int32 WriteRemainlen(uint8 *buf, uint32 len_val, uint16 write_pos)
{
	
	int32 remaining_count = 0;
	uint8 byte = 0;

	do
	{
		byte = len_val % 128;
		len_val = len_val >> 7;
		/* If there are more digits to encode, set the top bit of this digit */
		if (len_val > 0)
		{
			byte = byte | 0x80;
		}
		buf[write_pos++] = byte;
		remaining_count++;
	} while(len_val > 0 && remaining_count < 5);

	return --write_pos;
}

int32 ReadRemainlen(int8 *buf, uint32 *len_val, uint16 read_pos)
{
	
    uint32 multiplier = 1;
    uint32 len_len = 0;
    uint8 onebyte = 0;
	
    *len_val = 0;
	
    do
	{
        onebyte = buf[read_pos++];

        *len_val += (onebyte & 0x7f) * multiplier;
        multiplier <<= 7;

        len_len++;
        if (len_len > 4)
		{
            return -1;/*len of len more than 4;*/
        }
    } while((onebyte & 0x80) != 0);
	
    return read_pos;
}

//==========================================================
//	�������ƣ�	EDP_PacketSaveJson
//
//	�������ܣ�	��װЭ��ͷ
//
//	��ڲ�����	devid���豸ID(��Ϊ��)
//				send_len��json�ܳ�
//				type_bin_head��bin�ļ�����Ϣͷ
//				type������
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		����ΪType2��ʱ��type_bin_head��ΪNULL
//==========================================================
uint8 EDP_PacketSaveData(const int8 *devid, int32 send_len, int8 *type_bin_head, SaveDataType type, EDP_PACKET_STRUCTURE *edpPacket)
{

	int32 remain_len = 0;
	uint8 devid_len = strlen(devid);
	
	if(type == 2 && type_bin_head == NULL)
		return 1;
	
	if(type == 2)
		EDP_NewBuffer(edpPacket, strlen(type_bin_head) + 15);
	else
		EDP_NewBuffer(edpPacket, send_len + 24);
	if(edpPacket->_data == NULL)
		return 2;

	//Byte0����Ϣ����--------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = SAVEDATA;
	
	if(devid)
	{
		if(type == 2)
			remain_len = 12 + strlen(type_bin_head) + send_len;
		else
			remain_len = 8 + send_len + devid_len;
		
		//ʣ����Ϣ����-------------------------------------------------------------
		edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
		
		//��־--bit7:1-��devid��0-��devid		bit6:1-����Ϣ��ţ�0-����Ϣ���----
		edpPacket->_data[edpPacket->_len++] = 0xC0;
		
		//DEVID����---------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = 0;
		edpPacket->_data[edpPacket->_len++] = devid_len;
		
		//DEVID------------------------------------------------------------------
		strncat((int8 *)edpPacket->_data + edpPacket->_len, devid, devid_len);
		edpPacket->_len += devid_len;
		
		//��Ϣ���----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MSG_ID_HIGH;
		edpPacket->_data[edpPacket->_len++] = MSG_ID_LOW;
	}
	else
	{
		if(type == 2)
			remain_len = 10 + strlen(type_bin_head) + send_len;
		else
			remain_len = 6 + send_len;
		 
		//ʣ����Ϣ����------------------------------------------------------------
		edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
		
		//��־--bit7:1-��devid��0-��devid		bit6:1-����Ϣ��ţ�0-����Ϣ���----
		edpPacket->_data[edpPacket->_len++] = 0x40;
		
		//��Ϣ���----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MSG_ID_HIGH;
		edpPacket->_data[edpPacket->_len++] = MSG_ID_LOW;
	}
	
	edpPacket->_data[edpPacket->_len++] = type;
	
	if(type == 2)
	{
		uint8 type_bin_head_len = strlen(type_bin_head);
		uint8 i = 0;
		
		//��Ϣͷ����---------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(type_bin_head_len);
		edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(type_bin_head_len);
		
		//��Ϣͷ-------------------------------------------------------------------
		for(; i < type_bin_head_len; i++)
			edpPacket->_data[edpPacket->_len++] = type_bin_head[i];
		
		//ͼƬ����-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 24);
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 16);
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 8);
		edpPacket->_data[edpPacket->_len++] = (uint8)send_len;
	}
	else
	{
		//json����-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(send_len);
		edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(send_len);
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_PacketPushData
//
//	�������ܣ�	PushData�������
//
//	��ڲ�����	devid���豸ID
//				msg����������
//				msg_len�����͵����ݳ���
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8 EDP_PacketPushData(const int8 *devid, const int8 *msg, uint32 msg_len, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint32 remain_len = 2 + strlen(devid) + msg_len;
	uint8 devid_len = strlen(devid);
	uint16 i = 0;
	uint16 size = 5 + strlen(devid) + msg_len;
	
	if(devid == NULL || msg == NULL || msg_len == 0)
		return 1;

	EDP_NewBuffer(edpPacket, size);
	if(edpPacket->_data == NULL)
		return 2;
	
	//Byte0��pushdata����-----------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = PUSHDATA;
	
	//ʣ�೤��----------------------------------------------------------------------
	edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
	
	//DEVID����---------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(devid_len);
	edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(devid_len);
	
	//д��DEVID---------------------------------------------------------------------
	for(; i < devid_len; i++)
		edpPacket->_data[edpPacket->_len++] = devid[i];
	
	//д������----------------------------------------------------------------------
	for(i = 0; i < msg_len; i++)
		edpPacket->_data[edpPacket->_len++] = msg[i];
	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_UnPacketPushData
//
//	�������ܣ�	PushData���ܽ��
//
//	��ڲ�����	rev_data���յ�������
//				src_devid��Դdevid����
//				req�������
//				req_len�������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8 EDP_UnPacketPushData(uint8 *rev_data, int8 **src_devid, int8 **req, uint32 *req_len)
{

	int32 read_pos = 0;
	uint32 remain_len = 0;
	uint16 devid_len = 0;
	
	//Byte0��PushData��Ϣ------------------------------------------------------------
	if(rev_data[read_pos++] != PUSHDATA)
		return 1;

	//��ȡʣ�೤��--------------------------------------------------------------------
	read_pos = ReadRemainlen((int8 *)rev_data, &remain_len, read_pos);
	if(read_pos == -1)
		return 2;
	
	//��ȡԴdevid����-----------------------------------------------------------------
	devid_len = (uint16)rev_data[read_pos] << 8 | rev_data[read_pos + 1];
	read_pos += 2;

	//�����ڴ�------------------------------------------------------------------------
	*src_devid = (int8 *)EDP_MallocBuffer(devid_len + 1);
	if(*src_devid == NULL)
		return 3;

	//��ȡԴdevid---------------------------------------------------------------------
	memset(*src_devid, 0, devid_len + 1);
	memcpy(*src_devid, (const int8 *)rev_data + read_pos, devid_len);
	read_pos += devid_len;

	remain_len -= 2 + devid_len;

	//�����ڴ�------------------------------------------------------------------------
	*req = (int8 *)EDP_MallocBuffer(remain_len + 1);
	if(*req == NULL)
	{
		EDP_FreeBuffer(*src_devid);
		return 4;
	}

	//��ȡ����------------------------------------------------------------------------
	memset(*req, 0, remain_len + 1);
	memcpy(*req, (const int8 *)rev_data + read_pos, remain_len);
	read_pos += remain_len;

	*req_len = remain_len;
	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_UnPacketCmd
//
//	�������ܣ�	�·�������
//
//	��ڲ�����	rev_data���յ�������
//				cmdid��cmdid
//				cmdid_len��cmdid����
//				req������
//				req_len�������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint8 EDP_UnPacketCmd(uint8 *rev_data, int8 **cmdid, uint16 *cmdid_len, int8 **req, uint32 *req_len)
{

	uint32 remain_len = 0;
	int32 read_pos = 0;
	
	//Byte0��PushData��Ϣ------------------------------------------------------------
	if(rev_data[read_pos++] != CMDREQ)
		return 1;
	
	//��ȡʣ�೤��--------------------------------------------------------------------
	read_pos = ReadRemainlen((int8 *)rev_data, &remain_len, read_pos);
	if(read_pos == -1)
		return 2;
	
	//��ȡcmdid����-------------------------------------------------------------------
	*cmdid_len = (uint16)rev_data[read_pos] << 8 | rev_data[read_pos + 1];
	read_pos += 2;
	
	//�����ڴ�------------------------------------------------------------------------
	*cmdid = (int8 *)EDP_MallocBuffer(*cmdid_len + 1);
	if(*cmdid == NULL)
		return 3;
	
	//��ȡcmdid-----------------------------------------------------------------------
	memset(*cmdid, 0, *cmdid_len + 1);
	memcpy(*cmdid, (const int8 *)rev_data + read_pos, *cmdid_len);
	read_pos += *cmdid_len;
	
	//��ȡreq����---------------------------------------------------------------------
	*req_len = (uint32)rev_data[read_pos] << 24 | (uint32)rev_data[read_pos + 1] << 16
					 | (uint32)rev_data[read_pos + 2] << 8 | (uint32)rev_data[read_pos + 3];
	read_pos += 4;
	
	//�����ڴ�------------------------------------------------------------------------
	*req = (int8 *)EDP_MallocBuffer(*req_len + 1);
	if(*req == NULL)
	{
		EDP_FreeBuffer(*cmdid);
		return 4;
	}
	
	//��ȡreq-------------------------------------------------------------------------
	memset(*req, 0, *req_len + 1);
	memcpy(*req, (const int8 *)rev_data + read_pos, *req_len);
	read_pos += *req_len;
	
	return 0;

}

//==========================================================
//	�������ƣ�	EDP_PacketCmdResp
//
//	�������ܣ�	����ظ����
//
//	��ڲ�����	cmdid�������cmdid(�������·�)
//				cmdid_len��cmdid����
//				req������
//				req_len�������
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 EDP_PacketCmdResp(const int8 *cmdid, uint16 cmdid_len, const int8 *resp, uint32 resp_len, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint32 remain_len = cmdid_len + resp_len + (resp_len ? 6 : 2);
	
	EDP_NewBuffer(edpPacket, remain_len + 5);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0��CMDRESP��Ϣ------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = CMDRESP;
	
	//д��ʣ�೤��------------------------------------------------------------------
	edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
	
	//д��cmdid����------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = cmdid_len >> 8;
	edpPacket->_data[edpPacket->_len++] = cmdid_len & 0x00FF;
	
	//д��cmdid----------------------------------------------------------------------
	memcpy((int8 *)edpPacket->_data + edpPacket->_len, cmdid, cmdid_len);
	edpPacket->_len += cmdid_len;
	
	if(resp_len)
	{
		//д��req����-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 24);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 16);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 8);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len & 0x00FF);
		
		//д��req---------------------------------------------------------------------
		memcpy((int8 *)edpPacket->_data + edpPacket->_len, resp, resp_len);
		
		edpPacket->_len += resp_len;
	}

	return 0;

}

//==========================================================
//	�������ƣ�	EDP_PacketPing
//
//	�������ܣ�	�����������
//
//	��ڲ�����	edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 EDP_PacketPing(EDP_PACKET_STRUCTURE *edpPacket)
{

	EDP_NewBuffer(edpPacket, 2);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0��PINGREQ��Ϣ------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = PINGREQ;
	
	//Byte1��0----------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = 0;
	
	return 0;

}
