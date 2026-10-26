/* Copyright 2023 0x7ff
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "libdimentio.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Strict nonce argument parser: exactly "0x" + 1..16 hex digits. The old
 * sscanf("0x%016" PRIx64) silently truncated longer inputs and ignored
 * trailing garbage, and a failed parse failed silently. */
static bool
parse_nonce(const char *str, uint64_t *nonce) {
	size_t i, len;

	if(strncmp(str, "0x", 2) != 0) {
		return false;
	}
	len = strlen(str + 2);
	if(len == 0 || len > 16) {
		return false;
	}
	for(i = 0; i < len; ++i) {
		if(!isxdigit((unsigned char)str[2 + i])) {
			return false;
		}
	}
	*nonce = strtoull(str + 2, NULL, 16);
	return true;
}

int
main(int argc, char **argv) {
	uint8_t nonce_d[CC_SHA384_DIGEST_LENGTH];
	int ret = EXIT_FAILURE;
	size_t i, nonce_d_sz;
	uint64_t nonce;

	if(argc != 1 && argc != 2) {
		printf("Usage: %s [nonce]\n", argv[0]);
	} else if(argc == 2 && !parse_nonce(argv[1], &nonce)) {
		printf("Invalid nonce: %s\n", argv[1]);
		printf("Usage: %s [nonce]\n", argv[0]);
	} else {
		if(dimentio_preinit(&nonce, argc == 2, nonce_d, &nonce_d_sz) == KERN_SUCCESS || (dimentio_init(0, NULL, NULL) == KERN_SUCCESS && dimentio(&nonce, argc == 2, nonce_d, &nonce_d_sz) == KERN_SUCCESS)) {
			if(argc == 1) {
				printf("Current nonce is 0x%016" PRIX64 "\n", nonce);
			} else {
				printf("Set nonce to 0x%016" PRIX64 "\n", nonce);
			}
			if(nonce_d_sz != 0) {
				printf("nonce_d: ");
				for(i = 0; i < nonce_d_sz; ++i) {
					printf("%02" PRIX8, nonce_d[i]);
				}
				putchar('\n');
			}
			ret = 0;
		}
		dimentio_term();
	}
	return ret;
}
