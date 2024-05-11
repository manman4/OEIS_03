M=66;

a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, moebius(i*j*k))));
for(n=1, M, print1(a(n), ", "));

