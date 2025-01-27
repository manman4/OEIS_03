M=18;

\\ a(n+1) = n! * LaguerreL(n, 2*n+3, -1).
a(n) = if(n==0, 1, (n-1)!*pollaguerre(n-1, 2*n+1, -1));
for(n=0, M, print1(a(n), ", "));