\\ E.g.f. A(x) satisfies A(x) = exp(3*x*A(x)/(1-x*A(x))).
my(A=1, n=22); for(i=1, n, A=exp(3*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(A))

\\ E.g.f.: exp(3 * B(x)), where B(x) is the e.g.f. of A376094.
my(A=1, n=22); for(i=1, n, A=exp(3*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(log(A)/3))

\\ E.g.f.: C(x)^3, where C(x) is the e.g.f. of A361094.
my(A=1, n=22); for(i=1, n, A=exp(3*x*A / (1 - x*A) + x*O(x^n) ) ); Vec(serlaplace(A^(1/3)))
