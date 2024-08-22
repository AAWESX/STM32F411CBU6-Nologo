#include "str_control.h"
#include <stdio.h>
#include <ctype.h>

/**
 * @brief 将字符串转换为整数。
 * 
 * @param str 要转换的字符串
 * @return 转换后的整数值。如果字符串中的第一个字符不是数字，则返回 0。
 */
int My_atoi(const char *str)
{
    int result = 0;  // 存储转换后的整数值
    int sign = 1;    // 符号标志，默认为正数
    
    // 处理空字符串
    if (str == NULL) {
        return 0;  // 如果字符串为空，则返回 0
    }

    // 跳过前导空格
    while (isspace(*str)) {
        str++;
    }

    // 处理负号或正号
    if (*str == '-') {
        sign = -1;  // 设置符号为负数
        str++;      // 移动到下一个字符
    } else if (*str == '+') {
        str++;      // 移动到下一个字符，正号不改变符号
    }

    // 转换数字字符
    while (*str != '\0' && isdigit(*str)) {
        result = result * 10 + (*str - '0');  // 将字符转换为数字，并累加到结果中
        str++;  // 移动到下一个字符
    }

    // 返回结果，考虑符号
    return sign * result;
}


#define STEP_STRING_MAX_LEN 1024

/**
 * @brief 从以分隔符分隔的字符串中提取第 index 个字段
 * 
 * @param p_stepString  需要查找的字符串，包含以分隔符分隔的字段
 * @param index         需要提取的字段下标，1 表示第一个字段
 * @param delimiter     字段之间的分隔符
 * @param p_RxBuf       存储提取到的字段的缓冲区
 * 
 * @return int 函数执行结果，0 表示成功找到并提取字段，1 表示未找到指定的字段
 * 
 * @note 如果未找到指定的字段，p_RxBuf 将被置为空字符串。
 */
char Extract_Nth_Field(const char *p_stepString, int index, char delimiter, char *p_RxBuf) {
    int n = 0;           ///< 记录查找字符数
    int field_count = 0; ///< 记录字段数
    p_RxBuf[0] = '\0';  ///< 清空缓存区

    while (n < STEP_STRING_MAX_LEN && *p_stepString != '\0') {
        if ((*p_stepString == delimiter) || (n == 0)) {
            field_count++;
            if (field_count == index) { // 找到匹配下标
                if (n > 0) p_stepString++;
                while (*p_stepString != '\0' && *p_stepString != delimiter) {
                    *p_RxBuf = *p_stepString;
                    p_stepString++;
                    p_RxBuf++;
                }
                *p_RxBuf = '\0'; // 确保字符串以 null 结束
                return 0; // 成功找到字段
            }
        }
        n++;
        p_stepString++;
    }

    *p_RxBuf = '\0'; // 如果没有找到，返回空字符串
    return 1; // 找不到目标字段
}
