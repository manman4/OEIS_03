M=15;

a(n) = if(n<2, 1, -sum(k=1, n-1, 2^k*a(k)*a(n-k)));
for(n=0, M, print1(a(n), ", "));