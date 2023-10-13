M=27;

a(n) = if(n==0, 1, sum(k=1, sqrtint(n), binomial(n-1, k^2-1)*a(n-k^2))); 
for(n=0, M, print1(a(n), ", "));