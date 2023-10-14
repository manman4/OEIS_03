M=25;

a(n, s=4, t=1) = sum(k=0, n, (-k^t)^(n-k)*k^s);
for(n=0, M, print1(a(n), ", "));