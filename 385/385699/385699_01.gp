M=30;

\\ G.f. A(x) satisfies A(x) = 1/( 1 - x*(A(x) + A(-x))*(A(x) + A(w*x) + A(w^2*x))/6 ), where w = exp(2*Pi*i/3).
a(n) = my(A=1, w=exp(2*Pi*I/3)); for(i=1, n, A= 1/( 1 - x*(A + subst(A, x, -x))*(A + subst(A, x, w*x) + subst(A, x, w^2*x))/6 + x*O(x^n) )); polcoef(A, n);
for(n=0, M, print1(round(a(n)), ", "));

