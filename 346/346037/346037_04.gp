M=22;

a(n) = if(n==0, 1, (n-1)!*sum(k=1, n, k*sumdiv(k, d, 1/(d!*(k/d)!))*a(n-k)/(n-k)!));
for(n=0, M, print1(a(n), ", "));