M=47;

\\ G.f. A(x) satisfies A(x) = 1/(1 - x * Product_{k=0..5} A(w^k*x)), where w = exp(Pi*i/3).
a(n) = my(A=1, w=exp(Pi*I/3)); for(i=1, n, A= 1/(1- x*prod(k=0, 5, subst(A, x, w^k*x) ) + x*O(x^n) )); polcoef(A, n);
for(n=0, 15, print1(a(n), ", "));
for(n=0, M, print1(round(a(n)), ", "));
