M=21;
\\ a(n) = (1/(2*n)) * Sum_{k=1..n} k * ((-1)^k + Pell(k) + Pell(k+1)) * binomial(4*n,n-k) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
for(n=0, M, print1(pell(n), ", "));
a(n) = if(n==0, 1, sum(k=1, n, k * ((-1)^k + pell(k) + pell(k+1)) * binomial(4*n, n-k)) / (2*n));
for(n=0, M, print1(a(n),", "));

\\ a(n) = (A391465(n) - A006632(n))/2 for n > 0.
a391465(n) = if(n==0, 1, sum(k=1, n, k*(pell(k)+pell(k+1))*binomial(4*n, n-k))/n);
a006632(n) = 3*binomial(4*n-1, n-1)/(4*n-1);
b(n) = if(n==0, 1, (a391465(n) - a006632(n))/2);
for(n=0, 100, print1(a(n)-b(n),", "));
