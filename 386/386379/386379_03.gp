M=43;

\\ G.f. A(x) satisfies A(x) = 1/(1 - x * Product_{k=0..4} A(w^k*x)), where w = exp(2*Pi*i/5).
a(n) = my(A=1, w=exp(2*Pi*I/5)); for(i=1, n, A= 1/(1- x*prod(k=0, 4, subst(A, x, w^k*x) ) + x*O(x^n) )); polcoef(A, n);
for(n=0, 15, print1(a(n), ", "));
for(n=0, M, print1(round(a(n)), ", "));
