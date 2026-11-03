\\ G.f. A(x) satisfies A(x) = (1-3*x) * (2*A(x/(1+x)^4) - 1).
\\ A = R のとき A = 2*A - R
a(n) = my(A=1); for(i=1, n, A = 2*A - (1-3*x) * (2*subst(A, x, x/(1+x)^4) - 1 +x*O(x^n)) ); polcoeff(A, n);
for(n=0, 15, print1(a(n), ", "));

