\\ E.g.f. A(x) satisfies A(x) = exp( x*A(x) * A(2*x*A(x)) ).
a(n, k) = my(A=1); for(i=1, n, A = exp( x * A * subst(A, x, 2*x*A) )); n! * polcoef(A^k, n);
for(n=0, 15, print1(a(n, 1),", "));

