#include "NetworkPacket.h"

NetworkPacket::NetworkPacket()
{
    //isdevs = false;
    //devsCount = 0;
    //m_bRun1 = false;
}

NetworkPacket::~NetworkPacket()
{
    vecPacketData.clear();
    vector<std::string>().swap(vecPacketData);

    vecDevs.clear();
    vector<std::string>().swap(vecDevs);

    /* �ͷ��豸�б� */
    if (devsCount != 0)
    {
	    pcap_freealldevs(alldevs);
    }
}

int NetworkPacket::findAllDevs(std::vector<std::string> &vecDevs, char* errbuf)
{
    //pcap_if_t* alldevs;
    /* ��ȡ�����豸�б� */
    if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        //fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return WRONGNUM;
    }

    /* ��ӡ�б� */
    for (pDevs = alldevs; pDevs; pDevs = pDevs->next)
    {
        devsCount++;
        char devsListTemp[1024] = { 0 };
        //memcpy(devsListTemp, devs->next, strlen(devs->name));
        if (pDevs->description)
        {
            sprintf(devsListTemp, "%s", pDevs->description);
        }
        else
        {
            sprintf(devsListTemp, "%s(No description available)", pDevs->description);
        }
        ITC_WriteLog(LOG_LEVEL_INFO, "%s", devsListTemp);
        vecDevs.push_back(devsListTemp);
        setDevs(devsListTemp);
    }

    if (devsCount == 0)
    {
        memset(errbuf, 0x00, sizeof(errbuf));
        strcat(errbuf, "No interfaces found! Make sure WinPcap is installed.");
        return WRONGNUM;
    }
	return devsCount;
}

void NetworkPacket::setDevs(std::string strDevs)
{
    vecDevs.push_back(strDevs);
}

std::vector<std::string> NetworkPacket::getDevs()
{
    return vecDevs;
}

int NetworkPacket::openPacket(char* devsName, char* packetFilter, char* errbuf)
{

    ///* ��ȡ�����豸�б� */
    //if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    //{
    //    //fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
    //    return WRONGNUM;
    //}
    /* ��ת����ѡ�е������� */
    int i = 0;
    for (i = 0, pDevs = alldevs; i < devsCount-1; pDevs = pDevs->next, i++)
    {
        if (pDevs->name == NULL)
        {
            memset(errbuf, 0x00, sizeof(errbuf));
            strcat(errbuf, "No matching adapter found.");
            /* �ͷ��豸�б� */
            //pcap_freealldevs(alldevs);
            return WRONGNUM;
        }
        else if (strcmp(devsName, pDevs->description) == 0)
        {
            break;
        }
        
    }
    ITC_WriteLog(LOG_LEVEL_INFO, "��ǰ�򿪣�%s", pDevs->name);
    /* ���豸 */
    if ((adhandle = pcap_open(
        pDevs->name,          // �豸��
        65536,            // Ҫ��׽�����ݰ��Ĳ��� 
                          // 65535��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
        PCAP_OPENFLAG_PROMISCUOUS,    // ����ģʽ
        1000,             // ��ȡ��ʱʱ��
        NULL,             // Զ�̻�����֤
        errbuf            // ���󻺳��
    )) == NULL)
    {
        //fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", pDevs->name);

        /* �ͷ����б� */
        //pcap_freealldevs(alldevs);
        ITC_WriteLog(LOG_LEVEL_INFO, "Unable to open the adapter. %s is not supported by WinPcap", pDevs->name);
        return WRONGNUM;
    }


    /* ���ù����� */
    if (pDevs->addresses != NULL)
        /* ��ȡ�ӿڵ�һ����ַ������ */
        uintNetmask = ((struct sockaddr_in*)(pDevs->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* �������ӿ�û�е�ַ����ô���Ǽ�������ӿ���C�������� */
        uintNetmask = 0xffffff;
    if (pcap_compile(adhandle, &fcode, packetFilter, 1, uintNetmask) >= 0)
    {
        //���ù�����
        if (pcap_setfilter(adhandle, &fcode) < 0)
        {
            //fprintf(stderr, "\nError setting the filter.\n");
            memset(errbuf, 0x00, sizeof(errbuf));
            strcat(errbuf, "Error setting the filter.");
            ITC_WriteLog(LOG_LEVEL_INFO, "%s", errbuf);
            /* �ͷ��豸�б� */
            //pcap_freealldevs(alldevs);
            return WRONGNUM;
        }
    }
    else
    {
        memset(errbuf, 0x00, sizeof(errbuf));
        strcat(errbuf, "Error setting the filter.");
        ITC_WriteLog(LOG_LEVEL_INFO, "Error setting the filter.");
        //fprintf(stderr, "\nError setting the filter.\n");
        /* �ͷ��豸�б� */
        //pcap_freealldevs(alldevs);
        return WRONGNUM;
    }

    /* �ͷ��豸�б� */
    //pcap_freealldevs(alldevs);
    return 0;
}

int NetworkPacket::openPacket(int devsIndex, char* packetFilter, char* errbuf)
{
    //pcap_if_t* alldevs;
    ///* ��ȡ�����豸�б� */
    //if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    //{
    //    //fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
    //    return WRONGNUM;
    //}
    /* ��ת����ѡ�е������� */
    int i = 0;
    for (i = 0, pDevs = alldevs; i < devsIndex; pDevs = pDevs->next, i++);

    ITC_WriteLog(LOG_LEVEL_INFO, "��ǰ�����ƣ�%s", pDevs->name);
    ITC_WriteLog(LOG_LEVEL_INFO, "��ǰ��������%s", pDevs->description);
    /* ���豸 */
    if ((adhandle = pcap_open(
        pDevs->name,          // �豸��
        65536,            // Ҫ��׽�����ݰ��Ĳ��� 
                          // 65535��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
        PCAP_OPENFLAG_PROMISCUOUS,    // ����ģʽ
        1000,             // ��ȡ��ʱʱ��
        NULL,             // Զ�̻�����֤
        errbuf            // ���󻺳��
    )) == NULL)
    {
        //fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", pDevs->name);

        /* �ͷ����б� */
        //pcap_freealldevs(alldevs);
        ITC_WriteLog(LOG_LEVEL_INFO, "Unable to open the adapter. %s is not supported by WinPcap", pDevs->name);
        return WRONGNUM;
    }

    ITC_WriteLog(LOG_LEVEL_INFO, "�򿪳ɹ���%s(%s)", pDevs->name, pDevs->description);
    /* ���ù����� */
    if (pDevs->addresses != NULL)
        /* ��ȡ�ӿڵ�һ����ַ������ */
        uintNetmask = ((struct sockaddr_in*)(pDevs->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* �������ӿ�û�е�ַ����ô���Ǽ�������ӿ���C�������� */
        uintNetmask = 0xffffff;
    ITC_WriteLog(LOG_LEVEL_INFO, "��ȡ�ӿڵ�һ����ַ�����룺%d", uintNetmask);
    ITC_WriteLog(LOG_LEVEL_INFO, "���ù�������%s", packetFilter);
    if (pcap_compile(adhandle, &fcode, packetFilter, 1, uintNetmask) >= 0)
    {
        //���ù�����
        if (pcap_setfilter(adhandle, &fcode) < 0)
        {
            //fprintf(stderr, "\nError setting the filter.\n");
            memset(errbuf, 0x00, sizeof(errbuf));
            strcat(errbuf, "Error setting the filter.");
            ITC_WriteLog(LOG_LEVEL_INFO, "%s", errbuf);
            /* �ͷ��豸�б� */
            //pcap_freealldevs(alldevs);
            return WRONGNUM;
        }
    }
    else
    {
        memset(errbuf, 0x00, sizeof(errbuf));
        strcat(errbuf, "Error setting the filter.");
        ITC_WriteLog(LOG_LEVEL_INFO, "Error setting the filter.");
        //fprintf(stderr, "\nError setting the filter.\n");
        /* �ͷ��豸�б� */
        //pcap_freealldevs(alldevs);
        return WRONGNUM;
    }
    ITC_WriteLog(LOG_LEVEL_INFO, "���ù��������");
    /* �ͷ��豸�б� */
    //pcap_freealldevs(alldevs);
    return 0;
}




int NetworkPacket::packetStart()
{
    ITC_WriteLog(LOG_LEVEL_INFO, "��ʼ��ȡץ�����ݡ�����");
    m_bRun1 = true;
    std::thread proc1(&NetworkPacket::packetGetData,this);
    proc1.join();
    return 0;
}

int NetworkPacket::packetStop()
{
    ITC_WriteLog(LOG_LEVEL_INFO, "������ȡץ�����ݡ�����");
    m_bRun1 = false;

    return 0;
}

std::mutex m_mutex;
void NetworkPacket::packetGetData()
{

    /* ��ȡ���ݰ� */
    int res = 0, count = 0, i = 0;
    if ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0)
    {
        //std::lock_guard<std::mutex> g1(m_mutex);
        if (res == 0)
            /* ��ʱʱ�䵽 */
            return ;

        printf("%d.\n", ++count);
        /* ��ʱ���ת���ɿ�ʶ��ĸ�ʽ */
        local_tv_sec = header->ts.tv_sec;
        ltime = localtime(&local_tv_sec);
        strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);
        char times[256] = { 0 };
        sprintf(times, "%s,%.6d len:%d", timestr, header->ts.tv_usec, header->len);
        ITC_WriteLog(LOG_LEVEL_INFO, "%s", times);

        /* ��ӡ�� */
        char chRecvData[65535] = { 0 };
        memset(chRecvData, 0x00, sizeof(chRecvData));
        memcpy(chRecvData, pkt_data, header->caplen);
        sprintf(chRecvData, "%s", pkt_data + 54);
        setPacketData(chRecvData);
        int len = header->caplen + 1;
        CString strRecvData = "�����ƣ�";
        for (i = 1; i < len; i++)
        {
            CString recvTemp;
            recvTemp.Format("%.2x ", pkt_data[i - 1]);
            strRecvData.Append(recvTemp);

        }

        ITC_WriteLog(LOG_LEVEL_INFO, "%s", strRecvData);
        ITC_WriteLog(LOG_LEVEL_INFO, "%s", chRecvData);

    }

    if (res == -1) {
        //printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
        char chErr[256] = {};
        sprintf(chErr, "Error reading the packets: %s", pcap_geterr(adhandle));
        errStr = chErr;

    }
    if (!m_bRun1)
    {
        errStr = "ץ������";
    }
}

int NetworkPacket::getPacketDataLen()
{
    return vecPacketData.size();
}

std::string NetworkPacket::getPacketErr()
{
    return errStr;
}

void NetworkPacket::setPacketData(std::string strData)
{
    std::string XML_HEAD = "<MSG>";
    std::string XML_END = "</MSG>";

    for (;;)
    {
        if (strData.find(XML_HEAD) != -1 && strData.find(XML_END) != -1)
        {
            int a = strData.find(XML_HEAD);
            int b = strData.find(XML_END);
            if (a != -1 && b != -1)
            {
                std::string strUnpacking = strData.substr(a, b + 6);
                vecPacketData.push_back(strUnpacking);

                strData.erase(strData.find(XML_HEAD), strUnpacking.length());
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

std::vector<std::string> NetworkPacket::getPacketData()
{
    return vecPacketData;
}

