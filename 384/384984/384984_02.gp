M=17;

\\ E.g.f. A(x) satisfies A(x) = exp(x*A(x)/A(-x*A(x)^6)^3).
a(n, k) = my(A=1); for(i=1, n, A =exp(x*A / subst(A, x, -x*A^6)^3 + x*O(x^n)) ); n! * polcoef(A^k, n);
for(n=0, M, print1(a(n, 1),", "));

