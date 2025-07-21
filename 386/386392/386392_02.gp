M=40;

\\ G.f. A(x) satisfies A(x) = (1 + x*A(x)^(p/r))^r, where p=7, r=4.
my(A=1, n=M); for(i=1, n, A= (1 + x*A^(7/4))^4 + x*O(x^n) ); Vec(A)

\\ G.f.: B(x)^4, where B(x) is the g.f. of A002296.
my(A=1, n=M); for(i=1, n, A= (1 + x*A^(7/4))^4 + x*O(x^n) ); Vec(A^(1/4))
