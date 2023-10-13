M=19;

a(n) = sum(k=1, n, moebius(k)*k^n);
for(n=1, M, print1(a(n), ", "));