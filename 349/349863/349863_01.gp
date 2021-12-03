M=20;

a(n, s=2, t=2) = sum(k=0, n, (-k^t)^(n-k)*k^s);
for(n=0, M, print1(a(n), ", "));