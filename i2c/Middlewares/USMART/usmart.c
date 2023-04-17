/**
 ****************************************************************************************************
 * @file        usmart.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART ���ڵ������
 *
 *              USMART����ALIENTEK������һ�����ɵĴ��ڵ��Ի������,ͨ�� ��,�����ͨ���������ֵ��ó�
 *              ��������κκ���,��ִ��.���,�����������ĺ������������(֧������(10/16����,֧�ָ���)
 *              ���ַ�����������ڵ�ַ����Ϊ����),�����������֧��10���������,��֧�ֺ����� ��ֵ��ʾ.
 *              V2.1�汾�Ժ�����hex��dec����ָ��.���ǿ����������ú�����������ʾ��ʽ.Ҳ�����������ݵĽ�
 *              ��ת��,����:
 *              ����"hex 100"  ���ڴ��ڵ��������Ͽ��� HEX 0X64.
 *              ����"dec 0X64" ���ڴ��ڵ��������Ͽ��� DEC 100.
 *   @note
 *              USMART��Դռ�����@MDK 3.80A@2.0�汾��
 *              FLASH:4K~K�ֽ�(ͨ��USMART_USE_HELP��USMART_USE_WRFUNS����)
 *              SRAM:72�ֽ�(���ٵ������)
 *              SRAM���㹫ʽ:   SRAM=PARM_LEN+72-4  ����PARM_LEN������ڵ���4.
 *              Ӧ�ñ�֤��ջ��С��100���ֽ�.
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 * 
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵�� 
 * 
 * V3.4֮ǰ�汾��ϸ�޸�˵����USMART�ļ����µ�:readme.txt
 * 
 * V3.4 20200324
 * 1, ����usmart_port.c��usmart_port.h,���ڹ���USMART����ֲ,�����޸�
 * 2, �޸ı���������ʽΪ: uint8_t, uint16_t, uint32_t
 * 3, �޸�usmart_reset_runtimeΪusmart_timx_reset_time
 * 4, �޸�usmart_get_runtimeΪusmart_timx_get_time
 * 5, �޸�usmart_scan����ʵ�ַ�ʽ,�ĳ���usmart_get_input_string��ȡ������
 * 6, �޸�printf����ΪUSMART_PRINTF�궨��
 * 7, �޸Ķ�ʱɨ����غ���,���ú궨�巽ʽ,������ֲ
 *
 * V3.5 20201220
 * 1���޸Ĳ��ִ�����֧��AC6������
 *
 ****************************************************************************************************
 */

#include "./USMART/usmart.h"
#include "./USMART/usmart_str.h"
#include "./USMART/usmart_port.h"


/* ϵͳ���� */
char *sys_cmd_tab[] =
{
    "?",
    "help",
    "list",
    "id",
    "hex",
    "dec",
    "runtime",
};

/**
 * @brief       ����ϵͳָ��
 * @param       str : �ַ���ָ��
 * @retval      0,�ɹ�����;����,�������;
 */
uint8_t usmart_sys_cmd_exe(char *str)
{
    uint8_t i;
    char sfname[MAX_FNAME_LEN];                  /* ��ű��غ����� */
    uint8_t pnum;
    uint8_t rval;
    uint32_t res;
    res = usmart_get_cmdname(str, sfname, &i, MAX_FNAME_LEN);   /* �õ�ָ�ָ��� */

    if (res)return USMART_FUNCERR;                  /* �����ָ�� */

    str += i;

    for (i = 0; i < sizeof(sys_cmd_tab) / 4; i++)   /* ֧�ֵ�ϵͳָ�� */
    {
        if (usmart_strcmp(sfname, sys_cmd_tab[i]) == 0)break;
    }

    switch (i)
    {
        case 0:
        case 1: /* ����ָ�� */
            USMART_PRINTF("\r\n");
#if USMART_USE_HELP
            USMART_PRINTF("------------------------USMART V3.5------------------------ \r\n");
            USMART_PRINTF("    USMART����ALIENTEK������һ�����ɵĴ��ڵ��Ի������,ͨ�� \r\n");
            USMART_PRINTF("��,�����ͨ���������ֵ��ó���������κκ���,��ִ��.���,���\r\n");
            USMART_PRINTF("��������ĺ������������(֧������(10/16����,֧�ָ���)���ַ���\r\n"),
            USMART_PRINTF("��������ڵ�ַ����Ϊ����),�����������֧��10���������,��֧��\r\n"),
            USMART_PRINTF("��������ֵ��ʾ.֧�ֲ�����ʾ�������ù���,֧�ֽ���ת������.\r\n");
            USMART_PRINTF("����֧��:www.openedv.com\r\n");
            USMART_PRINTF("USMART��7��ϵͳ����(����Сд):\r\n");
            USMART_PRINTF("?:      ��ȡ������Ϣ\r\n");
            USMART_PRINTF("help:   ��ȡ������Ϣ\r\n");
            USMART_PRINTF("list:   ���õĺ����б�\r\n\n");
            USMART_PRINTF("id:     ���ú�����ID�б�\r\n\n");
            USMART_PRINTF("hex:    ����16������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
            USMART_PRINTF("dec:    ����10������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
            USMART_PRINTF("runtime:1,�����������м�ʱ;0,�رպ������м�ʱ;\r\n\n");
            USMART_PRINTF("�밴�ճ����д��ʽ���뺯�������������Իس�������.\r\n");
            USMART_PRINTF("--------------------------����ԭ��------------------------- \r\n");
#else
            USMART_PRINTF("ָ��ʧЧ\r\n");
#endif
            break;

        case 2: /* ��ѯָ�� */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------�����嵥--------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)USMART_PRINTF("%s\r\n", usmart_dev.funs[i].name);

            USMART_PRINTF("\r\n");
            break;

        case 3: /* ��ѯID */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------���� ID --------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)
            {
                usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &pnum, &rval); /* �õ����غ����� */
                USMART_PRINTF("%s id is:\r\n0X%08X\r\n", sfname, (unsigned int)usmart_dev.funs[i].func);  /* ��ʾID */
            }

            USMART_PRINTF("\r\n");
            break;

        case 4: /* hexָ�� */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* �������� */
            {
                i = usmart_str2num(sfname, &res);       /* ��¼�ò��� */

                if (i == 0) /* ����ת������ */
                {
                    USMART_PRINTF("HEX:0X%X\r\n", res); /* תΪ16���� */
                }
                else if (i != 4)return USMART_PARMERR;  /* ��������. */
                else        /* ������ʾ�趨���� */
                {
                    USMART_PRINTF("16���Ʋ�����ʾ!\r\n");
                    usmart_dev.sptype = SP_TYPE_HEX;
                }

            }
            else return USMART_PARMERR; /* ��������. */

            USMART_PRINTF("\r\n");
            break;

        case 5: /* decָ�� */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0)     /* �������� */
            {
                i = usmart_str2num(sfname, &res);       /* ��¼�ò��� */

                if (i == 0) /* ����ת������ */
                {
                    USMART_PRINTF("DEC:%lu\r\n", (unsigned long)res);  /* תΪ10���� */
                }
                else if (i != 4)
                {
                    return USMART_PARMERR;  /* ��������. */
                }
                else        /* ������ʾ�趨���� */
                {
                    USMART_PRINTF("10���Ʋ�����ʾ!\r\n");
                    usmart_dev.sptype = SP_TYPE_DEC;
                }

            }
            else 
            {
                return USMART_PARMERR;  /* ��������. */
            }
                
            USMART_PRINTF("\r\n");
            break;

        case 6: /* runtimeָ��,�����Ƿ���ʾ����ִ��ʱ�� */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* �������� */
            {
                i = usmart_str2num(sfname, &res);   /* ��¼�ò��� */

                if (i == 0) /* ��ȡָ����ַ���ݹ��� */
                {
                    if (USMART_ENTIMX_SCAN == 0)
                    {
                        USMART_PRINTF("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");/* ���� */
                    }
                    else
                    {
                        usmart_dev.runtimeflag = res;

                        if (usmart_dev.runtimeflag)
                        {
                            USMART_PRINTF("Run Time Calculation ON\r\n");
                        }
                        else 
                        {
                            USMART_PRINTF("Run Time Calculation OFF\r\n");
                        }
                    }
                }
                else 
                {
                    return USMART_PARMERR;  /* δ������,���߲������� */
                }
            }
            else 
            {
                return USMART_PARMERR;      /* ��������. */
            }
            
            USMART_PRINTF("\r\n");
            break;

        default:/* �Ƿ�ָ�� */
            return USMART_FUNCERR;
    }

    return 0;
}

/**
 * @brief       ��ʼ��USMART
 * @param       tclk: ��ʱ���Ĺ���Ƶ��(��λ:Mhz)
 * @retval      ��
 */
void usmart_init(uint16_t tclk)
{
#if USMART_ENTIMX_SCAN == 1
    usmart_timx_init(1000, tclk * 100 - 1);
#endif
    usmart_dev.sptype = 1;  /* ʮ��������ʾ���� */
}

/**
 * @brief       ��str�л�ȡ������,id,��������Ϣ
 * @param       str: �ַ���ָ��.
 * @retval      0,ʶ��ɹ�;����,�������.
 */
uint8_t usmart_cmd_rec(char *str)
{
    uint8_t sta, i, rval;   /* ״̬ */
    uint8_t rpnum, spnum;
    char rfname[MAX_FNAME_LEN];  /* �ݴ�ռ�,���ڴ�Ž��յ��ĺ����� */
    char sfname[MAX_FNAME_LEN];  /* ��ű��غ����� */
    sta = usmart_get_fname(str, rfname, &rpnum, &rval); /* �õ����յ������ݵĺ��������������� */

    if (sta)return sta; /* ���� */

    for (i = 0; i < usmart_dev.fnum; i++)
    {
        sta = usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &spnum, &rval); /* �õ����غ��������������� */

        if (sta)return sta; /* ���ؽ������� */

        if (usmart_strcmp(sfname, rfname) == 0) /* ��� */
        {
            if (spnum > rpnum)return USMART_PARMERR;/* ��������(���������Դ����������) */

            usmart_dev.id = i;  /* ��¼����ID. */
            break;  /* ����. */
        }
    }

    if (i == usmart_dev.fnum)return USMART_NOFUNCFIND;  /* δ�ҵ�ƥ��ĺ��� */

    sta = usmart_get_fparam(str, &i);   /* �õ������������� */

    if (sta)return sta;     /* ���ش��� */

    usmart_dev.pnum = i;    /* ����������¼ */
    return USMART_OK;
}

/**
 * @brief       USMARTִ�к���
 *   @note
 *              �ú�����������ִ�дӴ����յ�����Ч����.
 *              ���֧��10�������ĺ���,����Ĳ���֧��Ҳ������ʵ��.�����õĺ���.һ��5�����ҵĲ����ĺ����Ѿ����ټ���.
 *              �ú������ڴ��ڴ�ӡִ�����.��:"������(����1������2...����N)=����ֵ".����ʽ��ӡ.
 *              ����ִ�еĺ���û�з���ֵ��ʱ��,����ӡ�ķ���ֵ��һ�������������.
 *
 * @param       ��
 * @retval      ��
 */
void usmart_exe(void)
{
    uint8_t id, i;
    uint32_t res;
    uint32_t temp[MAX_PARM];        /* ����ת��,ʹ֧֮�����ַ��� */
    char sfname[MAX_FNAME_LEN];  /* ��ű��غ����� */
    uint8_t pnum, rval;
    id = usmart_dev.id;

    if (id >= usmart_dev.fnum)return;   /* ��ִ��. */

    usmart_get_fname((char *)usmart_dev.funs[id].name, sfname, &pnum, &rval);    /* �õ����غ�����,���������� */
    USMART_PRINTF("\r\n%s(", sfname);   /* �����Ҫִ�еĺ����� */

    for (i = 0; i < pnum; i++)      /* ������� */
    {
        if (usmart_dev.parmtype & (1 << i)) /* �������ַ��� */
        {
            USMART_PRINTF("%c", '"');
            USMART_PRINTF("%s", usmart_dev.parm + usmart_get_parmpos(i));
            USMART_PRINTF("%c", '"');
            temp[i] = (uint32_t) & (usmart_dev.parm[usmart_get_parmpos(i)]);
        }
        else    /* ���������� */
        {
            temp[i] = *(uint32_t *)(usmart_dev.parm + usmart_get_parmpos(i));

            if (usmart_dev.sptype == SP_TYPE_DEC)
            {
                USMART_PRINTF("%ld", (long)temp[i]);  /* 10���Ʋ�����ʾ */
            }
            else 
            {
                USMART_PRINTF("0X%X", temp[i]); /* 16���Ʋ�����ʾ */
            }
        }

        if (i != pnum - 1)USMART_PRINTF(",");
    }

    USMART_PRINTF(")");
#if USMART_ENTIMX_SCAN==1
    usmart_timx_reset_time();   /* ��ʱ������,��ʼ��ʱ */
#endif

    switch (usmart_dev.pnum)
    {
        case 0: /* �޲���(void����) */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)();
            break;

        case 1: /* ��1������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0]);
            break;

        case 2: /* ��2������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1]);
            break;

        case 3: /* ��3������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2]);
            break;

        case 4: /* ��4������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3]);
            break;

        case 5: /* ��5������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4]);
            break;

        case 6: /* ��6������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5]);
            break;

        case 7: /* ��7������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6]);
            break;

        case 8: /* ��8������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7]);
            break;

        case 9: /* ��9������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8]);
            break;

        case 10:/* ��10������ */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8], temp[9]);
            break;
    }

#if USMART_ENTIMX_SCAN==1
    usmart_timx_get_time(); /* ��ȡ����ִ��ʱ�� */
#endif

    if (rval == 1)  /* ��Ҫ����ֵ. */
    {
        if (usmart_dev.sptype == SP_TYPE_DEC)USMART_PRINTF("=%lu;\r\n", (unsigned long)res);   /* ���ִ�н��(10���Ʋ�����ʾ) */
        else USMART_PRINTF("=0X%X;\r\n", res);  /* ���ִ�н��(16���Ʋ�����ʾ) */
    }
    else USMART_PRINTF(";\r\n");    /* ����Ҫ����ֵ,ֱ��������� */

    if (usmart_dev.runtimeflag)     /* ��Ҫ��ʾ����ִ��ʱ�� */
    {
        USMART_PRINTF("Function Run Time:%d.%1dms\r\n", usmart_dev.runtime / 10, usmart_dev.runtime % 10);  /* ��ӡ����ִ��ʱ�� */
    }
}

/**
 * @brief       USMARTɨ�躯��
 *   @note
 *              ͨ�����øú���,ʵ��USMART�ĸ�������.�ú�����Ҫÿ��һ��ʱ�䱻����һ��
 *              �Լ�ʱִ�дӴ��ڷ������ĸ�������.
 *              �������������ж��������,�Ӷ�ʵ���Զ�����.
 *              ���������ԭ���û�,��USART_RX_STA��USART_RX_BUF[]��Ҫ�û��Լ�ʵ��
 *
 * @param       ��
 * @retval      ��
 */
void usmart_scan(void)
{
    uint8_t sta, len;
    char *pbuf = 0;

    pbuf = usmart_get_input_string();   /* ��ȡ���������� */
    if (pbuf == 0) return ; /* ��������, ֱ�ӷ��� */
     
    sta = usmart_dev.cmd_rec(pbuf);     /* �õ�����������Ϣ */

    if (sta == 0)
    {
        usmart_dev.exe();  /* ִ�к��� */
    }
    else
    {
        len = usmart_sys_cmd_exe(pbuf);

        if (len != USMART_FUNCERR)sta = len;

        if (sta)
        {
            switch (sta)
            {
                case USMART_FUNCERR:
                    USMART_PRINTF("��������!\r\n");
                    break;

                case USMART_PARMERR:
                    USMART_PRINTF("��������!\r\n");
                    break;

                case USMART_PARMOVER:
                    USMART_PRINTF("����̫��!\r\n");
                    break;

                case USMART_NOFUNCFIND:
                    USMART_PRINTF("δ�ҵ�ƥ��ĺ���!\r\n");
                    break;
            }
        }
    } 
 
}

#if USMART_USE_WRFUNS == 1  /* ���ʹ���˶�д���� */

/**
 * @brief       ��ȡָ����ַ��ֵ 
 * @param       ��
 * @retval      ��
 */ 
uint32_t read_addr(uint32_t addr)
{
    return *(uint32_t *)addr;
}

/**
 * @brief       ��ָ����ַд��ָ����ֵ
 * @param       ��
 * @retval      ��
 */ 
void write_addr(uint32_t addr, uint32_t val)
{
    *(uint32_t *)addr = val;
}

#endif





















