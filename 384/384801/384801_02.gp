M=10;

\\ E.g.f. A(x) satisfies A(x) = exp( x/A(-x*A(x)) ).
a(n, k) = my(A=1); for(i=1, n, A = exp( x / subst(A, x, -x*A) + x*O(x^n) )); n! * polcoef(A^k, n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

