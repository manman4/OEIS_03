M=16;

a(n, t=3) = sum(k=0, n, k^(t*(n-k)+k));
for(n=0, M, print1(a(n), ", "));