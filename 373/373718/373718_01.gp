\\ a(n) = Sum_{k=0..floor(3*n/7)} binomial(3*n-6*k,k).
a(n) = sum(k=0, 3*n\7, binomial(3*n-6*k, k));
for(n=0, 34, print1(a(n),", ")) 
