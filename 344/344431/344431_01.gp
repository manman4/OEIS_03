M=19;

a(n) = sum(k=1, n, moebius(k)*n^(k-1));
for(n=1, M, print1(a(n), ", "));