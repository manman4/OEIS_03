M=23;
\\ a(n) = (1/(4*n)) * Sum_{k=1..n} k * ((k+2)*Pell(k+1) + (k+1)*Pell(k+2)) * binomial(2*n,n-k) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, M, print1(pell(n), ", "));
a(n) = if(n==0, 1, sum(k=1, n, k * ((k+2)*pell(k+1) + (k+1)*pell(k+2)) * binomial(2*n, n-k)) / (4*n));
for(n=0, M, print1(a(n),", "));

