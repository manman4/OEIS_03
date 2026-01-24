M=10;

\\ column k is the expansion of e.g.f. 1/(1 - x*(-log(1-x))^k).
a(n, k) = my(x='x+O('x^(n+10))); n! * polcoef(1/(1 - x*(-log(1-x))^k), n);
for(n=0, M, for(k=0, n, print1(a(k,n-k), ", ")));