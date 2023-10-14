M=16;

a(n, s=0, t=4) = sum(k=0, n, (-k^t)^(n-k)*k^s);
for(n=0, M, print1(a(n), ", "));