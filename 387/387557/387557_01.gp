\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} 2^(n-3*k) * binomial(2*k+2,2*n-6*k+1).
a(n) = (1/2) * sum(k=0, n\3, 2^(n-3*k) * binomial(2*k+2,2*n-6*k+1));
for(n=0, 40, print1(a(n),", "))
