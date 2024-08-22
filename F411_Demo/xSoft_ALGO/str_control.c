#include "str_control.h"
#include <stdio.h>
#include <ctype.h>

/**
 * @brief ���ַ���ת��Ϊ������
 * 
 * @param str Ҫת�����ַ���
 * @return ת���������ֵ������ַ����еĵ�һ���ַ��������֣��򷵻� 0��
 */
int My_atoi(const char *str)
{
    int result = 0;  // �洢ת���������ֵ
    int sign = 1;    // ���ű�־��Ĭ��Ϊ����
    
    // ������ַ���
    if (str == NULL) {
        return 0;  // ����ַ���Ϊ�գ��򷵻� 0
    }

    // ����ǰ���ո�
    while (isspace(*str)) {
        str++;
    }

    // �����Ż�����
    if (*str == '-') {
        sign = -1;  // ���÷���Ϊ����
        str++;      // �ƶ�����һ���ַ�
    } else if (*str == '+') {
        str++;      // �ƶ�����һ���ַ������Ų��ı����
    }

    // ת�������ַ�
    while (*str != '\0' && isdigit(*str)) {
        result = result * 10 + (*str - '0');  // ���ַ�ת��Ϊ���֣����ۼӵ������
        str++;  // �ƶ�����һ���ַ�
    }

    // ���ؽ�������Ƿ���
    return sign * result;
}


#define STEP_STRING_MAX_LEN 1024

/**
 * @brief ���Էָ����ָ����ַ�������ȡ�� index ���ֶ�
 * 
 * @param p_stepString  ��Ҫ���ҵ��ַ����������Էָ����ָ����ֶ�
 * @param index         ��Ҫ��ȡ���ֶ��±꣬1 ��ʾ��һ���ֶ�
 * @param delimiter     �ֶ�֮��ķָ���
 * @param p_RxBuf       �洢��ȡ�����ֶεĻ�����
 * 
 * @return int ����ִ�н����0 ��ʾ�ɹ��ҵ�����ȡ�ֶΣ�1 ��ʾδ�ҵ�ָ�����ֶ�
 * 
 * @note ���δ�ҵ�ָ�����ֶΣ�p_RxBuf ������Ϊ���ַ�����
 */
char Extract_Nth_Field(const char *p_stepString, int index, char delimiter, char *p_RxBuf) {
    int n = 0;           ///< ��¼�����ַ���
    int field_count = 0; ///< ��¼�ֶ���
    p_RxBuf[0] = '\0';  ///< ��ջ�����

    while (n < STEP_STRING_MAX_LEN && *p_stepString != '\0') {
        if ((*p_stepString == delimiter) || (n == 0)) {
            field_count++;
            if (field_count == index) { // �ҵ�ƥ���±�
                if (n > 0) p_stepString++;
                while (*p_stepString != '\0' && *p_stepString != delimiter) {
                    *p_RxBuf = *p_stepString;
                    p_stepString++;
                    p_RxBuf++;
                }
                *p_RxBuf = '\0'; // ȷ���ַ����� null ����
                return 0; // �ɹ��ҵ��ֶ�
            }
        }
        n++;
        p_stepString++;
    }

    *p_RxBuf = '\0'; // ���û���ҵ������ؿ��ַ���
    return 1; // �Ҳ���Ŀ���ֶ�
}
