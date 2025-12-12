M=23;
\\ a(n) = (1/(2*n)) * Sum_{k=1..n} k * ((-1)^k + Pell(k) + Pell(k+1)) * binomial(3*n,n-k) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, M, print1(pell(n), ", "));
a(n) = if(n==0, 1, sum(k=1, n, k * ((-1)^k + pell(k) + pell(k+1)) * binomial(3*n, n-k)) / (2*n));
for(n=0, M, print1(a(n),", "));

