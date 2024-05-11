M=72;

a(n) = sum(j=1, n, sum(k=1, n, moebius(j*k)));
for(n=1, M, print1(a(n), ", "));

