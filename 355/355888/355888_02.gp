M=22;

a(n) = sum(k=1, n, sumdiv(k, d, d!));
for(n=1, M, print1(a(n), ", "));