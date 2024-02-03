\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n-1+k,n-4*k). 
a(n) = sum(k=0, n\4, binomial(n-1+k, n-4*k));
for(n=0, 34, print1(a(n), ", "));