\\ E.g.f. A(x) satisfies A(x) = exp(2*x*A(x)/(1-x*A(x))).
my(A=1, n=22); for(i=1, n, A=exp(2*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(A))

\\ E.g.f.: exp(2 * B(x)), where B(x) is the e.g.f. of A376093.
my(A=1, n=22); for(i=1, n, A=exp(2*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(log(A)/2))

\\ E.g.f.: C(x)^2, where C(x) is the e.g.f. of A361093.
my(A=1, n=22); for(i=1, n, A=exp(2*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(A^(1/2)))
