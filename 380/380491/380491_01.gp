M=18;

\\ a(n) = n! * LaguerreL(n, n-3, -1).
a(n) = n!*pollaguerre(n, n-3, -1);
for(n=0, M, print1(a(n), ", "));