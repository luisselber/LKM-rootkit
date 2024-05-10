#include <stdio.h>

void kernel_puc() {
    char *msg = "Kernel Implementation - PUC Campinas";

    printf("%s", msg);
}

int main() {
    printf("Hello World! I was compiled at: %s\n", __TIME__);

    kernel_puc();
    return 0;
}