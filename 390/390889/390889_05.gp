M=13;

\\ a(0) = 1; a(n) = 2 * Sum_{k=1..n} 5^(k-1) * binomial(n-1,k-1) * a(n-k).
a(n) = if(n==0, 1, 2*sum(k=1, n, 5^(k-1)*binomial(n-1,k-1) * a(n-k)) );
for(n=0, M, print1(a(n),", "));

