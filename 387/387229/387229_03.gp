M=20;

\\ a(n) = (1/4)^n * Sum_{k=0..n} 9^k * ((2*k+1) * (2*k+3)/3) * binomial(2*k,k) * binomial(2*(n-k),n-k)/(1-2*(n-k)).
a(n) = (1/4)^n * sum(k=0, n, 9^k * ((2*k+1) * (2*k+3)/3) * binomial(2*k,k) * binomial(2*(n-k),n-k)/(1-2*(n-k)));
for(n=0, M, print1(a(n), ", "));