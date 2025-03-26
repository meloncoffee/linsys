/*!
 * Copyright (C) 2025 JongHoon Shim <shim9532@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/*==== FILE DESCRIPTION =====================================================*/

/*!
 * @file    config.h
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.03.23
 * @brief   설정 헤더 파일
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/*==== INCLUDES =============================================================*/
/*==== GLOBAL DEFINES =======================================================*/

#ifdef LINSYS_DEFENDER
#define MODULE_NAME "linsys_defender"
#endif

/*==== GLOBAL STRUCTS =======================================================*/

/*!
 * @brief   전역 설정 정보 구조체
*/
typedef struct config {
    bool debug_mode;
} config_t;

/*==== GLOBAL VARIABLES =====================================================*/

extern config_t g_config;

/*==== GLOBAL FUNCTIONS DECLARATION =========================================*/

#endif /* _CONFIG_H */
