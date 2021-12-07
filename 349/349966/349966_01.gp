M=15;

a(n) = sum(k=0, n, (k*(n-k))^n);
for(n=0, M, print1(a(n), ", "));