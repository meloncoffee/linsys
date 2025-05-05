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
 * @file    config.h
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.05.05
 * @brief   설정 헤더 파일
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/*==== INCLUDES =============================================================*/

#include "common/type.h"

/*==== GLOBAL DEFINES =======================================================*/

#ifndef VERSION
#define VERSION "unknown"
#endif

#ifndef BUILD_TIME
#define BUILD_TIME "unknown"
#endif

#ifndef MODULE_NAME
#define MODULE_NAME "unknown"
#endif

/*==== GLOBAL STRUCTS =======================================================*/

/*!
 * @brief   전역 설정 정보 구조체
*/
typedef struct config {

} config_t;

/*!
 * @brief   런타임 설정 정보 구조체
*/
typedef struct runtime_config {

} runtime_config_t;

/*==== GLOBAL VARIABLES =====================================================*/

extern config_t g_config;
extern runtime_config_t g_runtime_config;

/*==== GLOBAL FUNCTIONS DECLARATION =========================================*/

#endif /* _CONFIG_H */
