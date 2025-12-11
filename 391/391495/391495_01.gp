M=20;
\\ a(n) = (1/(2*n)) * Sum_{k=1..n} k * (k*Pell(k+1) + (k+1)*Pell(k+2)) * binomial(4*n,n-k) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, M, print1(pell(n), ", "));
a(n) = if(n==0, 1, sum(k=1, n, k * (k*pell(k+1) + (k+1)*pell(k+2)) * binomial(4*n, n-k)) / (2*n));
for(n=0, M, print1(a(n),", "));

