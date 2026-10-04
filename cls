[1mdiff --git a/user/sixfive.c b/user/sixfive.c[m
[1mindex bc5a91f..e9fa131 100644[m
[1m--- a/user/sixfive.c[m
[1m+++ b/user/sixfive.c[m
[36m@@ -2,13 +2,11 @@[m
 #include "user/user.h"[m
 #include "kernel/fcntl.h"[m
 [m
[31m-#define MAX_BUF_SIZE 40[m
[31m-[m
 int main(int argc, char *argv[])[m
 {[m
 	if (argc < 2)[m
 	{[m
[31m-		fprintf(stderr, "sixfive usage: sixfive [filenames...] \n");[m
[32m+[m		[32mfprintf(2, "sixfive usage: sixfive [filenames...] \n");[m[41m[m
 		exit(1);[m
 	}[m
 [m
[36m@@ -18,66 +16,58 @@[m [mint main(int argc, char *argv[])[m
 [m
 		if (fd == -1)[m
 		{[m
[31m-			fprintf(stderr, "cannot open file %s \n", argv[i]);[m
[32m+[m			[32mfprintf(2, "cannot open file %s \n", argv[i]);[m[41m[m
 			continue;[m
 		}[m
 [m
 		char valid_char[] = " -\r\t\n./,";[m
 		char c;[m
[31m-[m
[31m-		char buf[MAX_BUF_SIZE];[m
[31m-		int n = 0;[m
[32m+[m		[32mint n;[m[41m[m
 		int num = 0;[m
[31m-		int buf_len = 0;[m
[31m-		bool start_valid = true;[m
[32m+[m		[32mint have_digits = 0;[m[41m[m
[32m+[m		[32mint too_large = 0;[m[41m[m
[32m+[m		[32mint start_valid = 1;[m[41m[m
 [m
[31m-		memset(buf, 0, MAX_BUF_SIZE);[m
[31m-		buf[0] = '\0';[m
[31m-[m
[31m-		while ((n = read(fd, &c, 1)) >= 0)[m
[32m+[m		[32mwhile ((n = read(fd, &c, 1)) > 0)[m[41m[m
 		{[m
 			char* is_valid = strchr(valid_char, c);[m
[31m-			if (is_valid || n == 0)[m
[32m+[m			[32mif (is_valid)[m[41m[m
 			{[m
[31m-				start_valid = true;[m
[31m-				if (buf[0] != '\0')[m
[31m-				{[m
[31m-					//_sizeof = 0;[m
[31m-					buf[buf_len] = '\0';[m
[31m-					num = atoi(buf);[m
[31m-					if (num % 5 == 0 || num % 6 == 0)[m
[31m-					{[m
[31m-						fprintf(stdout, "%d \n", num);[m
[31m-					}[m
[31m-				}[m
[32m+[m				[32mif (have_digits && !too_large && (num % 5 == 0 || num % 6 == 0))[m[41m[m
[32m+[m					[32mfprintf(1, "%d \n", num);[m[41m[m
 [m
[31m-				memset(buf, 0, MAX_BUF_SIZE);[m
[31m-				buf[0] = '\0';[m
[31m-				buf_len = 0;[m
[32m+[m				[32mstart_valid = 1;[m[41m[m
[32m+[m				[32mnum = 0;[m[41m[m
[32m+[m				[32mhave_digits = 0;[m[41m[m
[32m+[m				[32mtoo_large = 0;[m[41m[m
 			}[m
[31m-[m
[31m-			if (!is_valid)[m
[32m+[m			[32melse if (c >= '0' && c <= '9' && start_valid)[m[41m[m
 			{[m
[31m-				if ('0' <= c && c <= '9' && start_valid)[m
[32m+[m				[32mint digit = c - '0';[m[41m[m
[32m+[m				[32mhave_digits = 1;[m[41m[m
[32m+[m				[32mif (!too_large)[m[41m[m
 				{[m
[31m-					buf[buf_len] = c;[m
[31m-					buf_len++;[m
[31m-				}[m
[31m-				else[m
[31m-				{[m
[31m-					start_valid = false;[m
[31m-					if (buf[0] != '\0')[m
[31m-					{[m
[31m-						memset(buf, 0, MAX_BUF_SIZE);[m
[31m-						buf[0] = '\0';[m
[31m-						buf_len = 0;[m
[31m-					}[m
[32m+[m					[32mif (num > (2147483647 - digit) / 10)[m[41m[m
[32m+[m						[32mtoo_large = 1;[m[41m[m
[32m+[m					[32melse[m[41m[m
[32m+[m						[32mnum = num * 10 + digit;[m[41m[m
 				}[m
 			}[m
[31m-[m
[31m-			if (n == 0)[m
[31m-				break;[m
[32m+[m			[32melse[m[41m[m
[32m+[m			[32m{[m[41m[m
[32m+[m				[32mstart_valid = 0;[m[41m[m
[32m+[m				[32mnum = 0;[m[41m[m
[32m+[m				[32mhave_digits = 0;[m[41m[m
[32m+[m				[32mtoo_large = 0;[m[41m[m
[32m+[m			[32m}[m[41m[m
 		}[m
[32m+[m[41m[m
[32m+[m		[32mif (n < 0)[m[41m[m
[32m+[m			[32mfprintf(2, "cannot read file %s \n", argv[i]);[m[41m[m
[32m+[m		[32melse if (have_digits && !too_large && (num % 5 == 0 || num % 6 == 0))[m[41m[m
[32m+[m			[32mfprintf(1, "%d \n", num);[m[41m[m
[32m+[m[41m[m
[32m+[m		[32mclose(fd);[m[41m[m
 	}[m
 [m
 	exit(0);[m
