\\ G.f. A(x) satisfies A(x) = 1 + x * A(x^2/(1 - x)^2) / (1 - x)^3.
M=31;

a(n)=my(A=1); for(i=1, n, A=1+x*subst( A, x, x^2/(1-x+x*O(x^n))^2 )/(1-x+x*O(x^n))^3); polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));

\\ E.g.f.: 1 + d/dx ( exp(x) * Sum_{k>=0} a(k) * x^(2*k+2) / (2*k+2)! ).
Vec(serlaplace(1 + deriv( exp(x + x*O(x^M)) * sum(k=0, M, a(k) * x^(2*k+2) / (2*k+2)!) )))