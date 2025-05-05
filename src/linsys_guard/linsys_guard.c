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
 * @file    linsys_guard.c
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.05.05
 * @brief   linsys_guard 메인 소스 파일
*/

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "common/config.h"
#include "common/define.h"
#include "common/process.h"

/*==== LOCAL DEFINES ========================================================*/
/*==== LOCAL STRUCTS ========================================================*/
/*==== LOCAL VARIABLES ======================================================*/

static struct option g_options[] = {
    {"help",    no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0}
};

config_t g_config;
runtime_config_t g_runtime_config;

/*==== LOCAL FUNCTIONS DECLARATION ==========================================*/

static void help(void);
static void version(void);

/*==== FUNCTIONS ============================================================*/

/*!
 * @brief   linsys_guard 메인 함수
 * @param   argc: 인자값 개수
 * @param   argv: 인자값
 * @return  int: 정상 종료(0), 비정상 종료(other)
*/
int main(int argc, char *argv[])
{
    int opt = -1;

    if ( argc <= 1 ) {
        help();
        exit(0);
    }

    /* 명령행 옵션 처리 */
    while ((opt = getopt_long(argc, argv, "hv", g_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                help();
                exit(0);
            case 'v':
                version();
                exit(0);
            case '?':
            default:
                help();
                exit(0);
        }
    }

    /* 명령어 존재유무 체크 */
    if (optind >= argc) {
        fprintf(stderr, "[WARN] Please input command: start|stop|debug\n");
        exit(0);
    }

    /* 모듈 가동 */
    if (0 == strcmp(argv[optind], "start")) {
        
    }
    /* 모듈 디버그 모드 가동 */
    else if (0 == strcmp(argv[optind], "debug")) {

    }
    /* 모듈 정지 */
    else if (0 == strcmp(argv[optind], "stop")) {

    }
    else {
        fprintf(stderr, "[WARN] Invalid Command: %s\n", argv[optind]);
        exit(0);
    }

    /* 현재 실행 중인 프로세스의 경로를 작업 디렉토리로 변경 */
    if (-1 == change_working_directory()) {
        fprintf(stderr, "[ERROR] Failed to change working directory\n");
        exit(1);
    }

    return 0;
}

/*!
 * @brief   도움말 출력
*/
static void help(void)
{
    printf("Usage: %s [command] [options]\n", MODULE_NAME);
    printf("Command: start|stop|debug\n");
    printf("Options:\n");
    printf("  -h, --help        show usage\n");
    printf("  -v, --version     show version\n");
}

/*!
 * @brief   버전 정보 출력
*/
static void version(void)
{
    printf("Build date: %s\n", BUILD_TIME);
    printf("%s version %s\n", MODULE_NAME, VERSION);
}
