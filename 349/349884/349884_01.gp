M=19;

a(n, t=2) = sum(k=0, n, (-k^t)^(n-k)*k^k);
for(n=0, M, print1(a(n), ", "));