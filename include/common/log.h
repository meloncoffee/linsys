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
 * @file    log.h
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.05.05
 * @brief   로그 처리 기능 헤더 파일
*/

#ifndef _LOG_H
#define _LOG_H

/*==== INCLUDES =============================================================*/
/*==== GLOBAL DEFINES =======================================================*/

#define PRINT_INFO(_fmt_, ...) fprintf(stdout, "[INFO] " _fmt_ "\n", ##__VA_ARGS__)
#define PRINT_WARN(_fmt_, ...) fprintf(stderr, "[WARN] " _fmt_ "\n", ##__VA_ARGS__)
#define PRINT_ERROR(_fmt_, ...) fprintf(stderr, "[ERROR] " _fmt_ "\n", ##__VA_ARGS__)

/*==== GLOBAL STRUCTS =======================================================*/
/*==== GLOBAL VARIABLES =====================================================*/
/*==== GLOBAL FUNCTIONS DECLARATION =========================================*/

#endif /* _LOG_H */
