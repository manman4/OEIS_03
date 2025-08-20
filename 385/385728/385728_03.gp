\\ a(n) = (1/2)^n * Sum_{k=0..n} 3^k * (2*k+1) * (2*(n-k)+1) * binomial(2*k,k) * binomial(2*(n-k),n-k).
a(n) = (1/2)^n * sum(k=0, n, 3^k * (2*k+1) * (2*(n-k)+1) * binomial(2*k,k) * binomial(2*(n-k),n-k));
for(n=0, 19, print1(a(n),", "));
