M=16;

a(n) = if(n==0, 1, sum(k=0, n, (n+(n-1)*(k+1))!/(n*k+n-1)! * binomial(n,k)));
for(n=0, M, print1(a(n), ", "));
