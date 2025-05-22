M=20;

\\ A(n,k) = n! * [x^n] (1 + k*x)^(n/k).
a(n, k) = my(x='x+O('x^(n+1))); n! * polcoef( (1 + k*x)^(n/k), n);
for(n=0, M, for(k=0, n-1, print1(a(k,n-k),", ")));
