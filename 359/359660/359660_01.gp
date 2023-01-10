M=19;

a(n, s=1, t=2) = sum(k=0, n, k^(t*(n-k)+s));
for(n=0, M, print1(a(n), ", "));