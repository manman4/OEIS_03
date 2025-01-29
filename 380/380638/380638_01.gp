M=15;

\\ a(n+1) = 4^n * n! * LaguerreL(n, 3*n+4, -1/4).
a(n) = if(n==0, 1, 4^(n-1) * (n-1)! * pollaguerre(n-1, 3*n+1, -1/4));
for(n=0, M, print1(a(n), ", "));