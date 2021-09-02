/*

MIT License

Copyright (c) 2015-2020 Johan Manuel, et. al.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdio.h>
#include <stdbool.h>

#define BUF_SIZE 512

bool cat(const char* path) {
    char buf[BUF_SIZE] = "";
    FILE* f = fopen(path, "r");

    if (!f) {
        return false;
    }

    while (true) {
        int read = fread(buf, 1, BUF_SIZE - 1, f);
        buf[read] = '\0';

        printf("%s", buf);

        if (read < BUF_SIZE - 1) {
            break;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s FILE...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (!cat(argv[i])) {
            printf("%s: failed to open '%s'\n", argv[0], argv[i]);
            return 2;
        }
    }

    return 0;
}
