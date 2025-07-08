M=25;

\\ E.g.f. A(x) satisfies A(x) = exp( x*(A(x) + A(i*x) + A(-x) + A(-i*x))/4 ), where i is the imaginary unit.
a(n) = my(A=1); for(i=1, n, A= exp( x*(A + subst(A, x, I*x) + subst(A, x, -x) + subst(A, x, -I*x))/4 + x*O(x^n) )); n! * polcoef(A, n);
for(n=0, M, print1(a(n), ", "));

