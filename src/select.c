#include "select.h"

extern fd_set READSET;   

/**
 * @brief 初始化select
 *
 */
void init_select(){

    FD_ZERO(&READSET);  // 初始化监听集合
    FD_SET(STDIN_FILENO, &READSET); // 将标准输入添加到监听集合中

}