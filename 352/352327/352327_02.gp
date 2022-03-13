M=19;

a(n) = if(n==0, 1, sum(k=1, n, (3+(-1)^k)/2*binomial(n, k)*a(n-k)));
for(n=0, M, print1(a(n), ", "));