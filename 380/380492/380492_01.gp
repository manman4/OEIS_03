M=18;

\\ a(n) = n! * LaguerreL(n, n-2, -1).
a(n) = n!*pollaguerre(n, n-2, -1);
for(n=0, M, print1(a(n), ", "));