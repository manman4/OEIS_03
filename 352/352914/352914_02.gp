M=19;

a(n) = if(n==0, 1, (n-1)!*sum(k=1, n, k*prime(k)*a(n-k)/(n-k)!));
for(n=0, M, print1(a(n), ", "));