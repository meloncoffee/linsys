/*!
 * Copyright (C) 2025 LINSYS Project Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/*==== FILE DESCRIPTION =====================================================*/

/*!
 * @file    process.c
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.05.05
 * @brief   공용 프로세스 관련 기능 소스 파일
*/

/*==== INCLUDES =============================================================*/

#include <unistd.h>
#include <libgen.h>
#include "common/process.h"
#include "common/define.h"

/*==== LOCAL DEFINES ========================================================*/
/*==== LOCAL STRUCTS ========================================================*/
/*==== LOCAL VARIABLES ======================================================*/

/*==== LOCAL FUNCTIONS DECLARATION ==========================================*/

/*==== FUNCTIONS ============================================================*/

/*!
 * @brief   현재 프로세스의 실행파일이 위치한 경로로 작업 경로 변경
 * @return  int: 성공(0), 실패(-1)
*/
int change_working_directory(void)
{   
    char exe_path[BUF_SIZE_1K] = {0};
    char *dir_path = NULL;

    /* /proc/self/exe에서 실행 파일의 전체 경로 획득 */
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (-1 == len) {
        return -1;
    }

    /* 실행 파일의 디렉토리 경로 추출 */
    dir_path = dirname(exe_path);

    // 작업 디렉토리 변경
    if (-1 == chdir(dir_path)) {
        return -1;
    }

    return 0;
}
