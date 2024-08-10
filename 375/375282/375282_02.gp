\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*n-6*k,2*k).
a(n) = sum(k=0, n\4, binomial(2*n-6*k, 2*k));
for(n=0, 36, print1(a(n),", ")) 
