M=36;

a(n) = sum(k=1, n, moebius(k)*2^(n-k));
for(n=0, M, print1(a(n), ", "));