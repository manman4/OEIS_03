M=17;

\\ a(n+1) = 2^n * n! * LaguerreL(n, n+2, -1/2).
a(n) = if(n==0, 1, 2^(n-1) * (n-1)! * pollaguerre(n-1, n+1, -1/2));
for(n=0, M, print1(a(n), ", "));