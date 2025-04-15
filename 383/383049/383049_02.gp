M=10;

\\ E.g.f. of column k: Sum_{j>=0} (j+1)^k * log(1+x)^j / j!.
a(n, k) = my(x='x+O('x^(n+1))); n! * polcoef(sum(j=0, n, (j+1)^k * log(1+x)^j/j!), n);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));

