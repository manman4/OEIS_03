M=27;

a(n, s=2, t=1) = sum(k=0, n, (-k^t)^(n-k)*k^s);
for(n=0, M, print1(a(n), ", "));