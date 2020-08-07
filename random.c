#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <stdio.h>

int position(long * w, int n, int * present, long find) {
    if(n == 0) return 0;
    int mid = n/2;
    if(w[mid] == find) {
        *present = 1;
        return mid;
    }
    if(w[mid] < find) {
        return mid + position(w + mid, n - mid, present, find);
    }
    return position(w, mid, present, find);
}

void query(long * w, int n) {
    int present, index;
    long x, y, find;
    present = 0;
    scanf("%ld %ld\n", &x, &y);
    find = x + y;
    index = position(w, n, &present, find);
    if(present) {
        printf("%d\n", -1);
        return;
    }
    printf("%d\n", index);
}

void run() {
    int n, q;
	scanf("%d\n", &n);
    long w[n];
    for(long i = 0; i < n; i++) scanf("%ld", w+i);
	scanf("%d\n", &q);
    for(int i = 0; i < q; i++) query(w, n);
}

int main(void) {
	int t;
	return 0;
}

