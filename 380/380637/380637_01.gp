M=15;

\\ a(n+1) = 3^n * n! * LaguerreL(n, 2*n+3, -1/3).
a(n) = if(n==0, 1, 3^(n-1) * (n-1)! * pollaguerre(n-1, 2*n+1, -1/3));
for(n=0, M, print1(a(n), ", "));