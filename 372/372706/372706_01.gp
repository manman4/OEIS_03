M=92;

a(n) = sum(k=1, n, moebius(n*k));
for(n=1, M, print1(a(n), ", "));

