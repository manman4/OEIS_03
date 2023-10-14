M=23;

a(n, s=3, t=1) = sum(k=0, n, k^(t*(n-k)+s));
for(n=0, M, print1(a(n), ", "));