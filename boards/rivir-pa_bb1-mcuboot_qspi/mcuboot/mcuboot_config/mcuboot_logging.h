// Copyright © RIVIR rivir.space

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __MCUBOOT_LOGGING_H__
#define __MCUBOOT_LOGGING_H__

#include <stdio.h>
#include <stdint.h>

#define MCUBOOT_LOG_LEVEL_OFF      0
#define MCUBOOT_LOG_LEVEL_ERROR    1
#define MCUBOOT_LOG_LEVEL_WARNING  2
#define MCUBOOT_LOG_LEVEL_INFO     3
#define MCUBOOT_LOG_LEVEL_DEBUG    4
#define MCUBOOT_LOG_LEVEL_SIM      5  /* RUST_LOG=trace */

/*
 * The compiled log level determines the maximum level that can be
 * printed.  Messages at or below this level can be printed, provided
 * they are also enabled through the Rust logging system, such as by
 * setting RUST_LOG to bootsim::api=info.
 */
#ifndef MCUBOOT_LOG_LEVEL
#define MCUBOOT_LOG_LEVEL MCUBOOT_LOG_LEVEL_DEBUG
#endif

#define MCUBOOT_LOG_MODULE_DECLARE(domain)	/* ignore */
#define MCUBOOT_LOG_MODULE_REGISTER(domain)	/* ignore */


uint32_t usart_putdata(void const *data, uint32_t length);
#define MAX_MSG  128

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_ERROR
#define MCUBOOT_LOG_ERR(_fmt, ...)                                      \
    do {                                                                \
    	char msg[MAX_MSG];                                              \
    	snprintf(msg, MAX_MSG-1, "ERR:" _fmt "\n", ##__VA_ARGS__);       \
		usart_putdata(msg, strlen(msg));                                \
    } while (0)
#else
#define MCUBOOT_LOG_ERR(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_WARNING
#define MCUBOOT_LOG_WRN(_fmt, ...)                                      \
    do {                                                                \
    	char msg[MAX_MSG];                                              \
    	snprintf(msg, MAX_MSG-1, "WARN:" _fmt "\n", ##__VA_ARGS__);     \
		usart_putdata(msg, strlen(msg));                                \
    } while (0)
#else
#define MCUBOOT_LOG_WRN(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_INFO
#define MCUBOOT_LOG_INF(_fmt, ...)                                      \
    do {                                                                \
    	char msg[MAX_MSG];                                              \
    	snprintf(msg, MAX_MSG-1, "INFO:" _fmt "\n", ##__VA_ARGS__);       \
		usart_putdata(msg, strlen(msg));                                \
    } while (0)
#else
#define MCUBOOT_LOG_INF(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_DEBUG
#define MCUBOOT_LOG_DBG(_fmt, ...)                                      \
    do {                                                                \
    	char msg[MAX_MSG];                                              \
    	snprintf(msg, MAX_MSG-1, "DEBUG:" _fmt "\n", ##__VA_ARGS__);       \
		usart_putdata(msg, strlen(msg));                                \
    } while (0)
#else
#define MCUBOOT_LOG_DBG(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_SIM
#define MCUBOOT_LOG_SIM(_fmt, ...)                                      \
    do {                                                                \
    	char msg[MAX_MSG];                                              \
    	snprintf(msg, MAX_MSG-1, "SIM:" _fmt "\n", ##__VA_ARGS__);       \
		usart_putdata(msg, strlen(msg));                                \
    } while (0)
#else
#define MCUBOOT_LOG_SIM(...) IGNORE(__VA_ARGS__)
#endif

#endif /* __MCUBOOT_LOGGING_H__ */
