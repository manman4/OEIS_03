\\ E.g.f. A(x) satisfies A(x) = exp( x * A(x*A(x)^3) ).
a(n, k) = my(A=1); for(i=1, n, A = exp( x * subst(A, x, x*A^3) )); n! * polcoef(A^k, n);
for(n=0, 15, print1(a(n, 1),", "));

