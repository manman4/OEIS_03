M=17;

\\ a(n) = 1/(n+1) * Sum_{k=0..n+1} 2^k * (-1)^(n+1-k) * k^n * binomial(n+1,k).
a(n) = (1/(n+1)) * sum(k=0, n+1, 2^k * (-1)^(n+1-k) * binomial(n+1,k) * k^n);
for(n=0, M, print1(a(n),", "))   
