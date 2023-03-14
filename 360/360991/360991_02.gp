M=23;

a(n) = if(n<2, 1, (n-1) * a(n-2) + sum(k=1, n, binomial(n-1,k-1) * a(n-k)));
for(n=0, M, print1(a(n), ", "));


