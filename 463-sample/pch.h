/* HW1-1
 * pch.h: This is a precompiled header file.
 * CSCE 463 Sample Code
 * by Dmitri Loguinov
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter
 * with permission for HW1-1
 * Files listed below are compiled only once, improving build performance for future builds.
 * This also affects IntelliSense performance, including code completion and many code browsing features.
 * However, files listed here are ALL re-compiled if any one of them is updated between builds.
 * Do not add files here that you will be updating frequently as this negates the performance advantage.
 */

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "DNS.h"
#include "cpu.h"

#include "HTMLParserBase.h"
#include "url_parser.h"

//#define DEBU
//#define NO_QUIT // TODO: Think I found an issue where I am exiting when I need to be returning null, I am not freeing memory that I have allocated or need some way to do this. I need some way to gracefully exit

#endif //PCH_H
