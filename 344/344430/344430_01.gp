M=18;

a(n) = sum(k=1, n, moebius(k)*k^k);
for(n=1, M, print1(a(n), ", "));