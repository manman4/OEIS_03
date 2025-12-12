M=23;
\\ a(n) = (1/(2*n)) * Sum_{k=1..n} k * ((-1)^k + Pell(k) + Pell(k+1)) * binomial(3*n,n-k) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, M, print1(pell(n), ", "));
a(n) = if(n==0, 1, sum(k=1, n, k * ((-1)^k + pell(k) + pell(k+1)) * binomial(3*n, n-k)) / (2*n));
for(n=0, M, print1(a(n),", "));

\\ a(n) = (A391462(n) - A006013(n-1))/2 for n > 0.
a391462(n) = if(n==0, 1, sum(k=1, n, k*(pell(k)+pell(k+1))*binomial(3*n, n-k))/n);
a006613(n) = binomial(3*n+1,n)/(n+1);
b(n) = if(n==0, 1, (a391462(n) - a006613(n-1))/2);
for(n=0, 100, print1(a(n)-b(n),", "));