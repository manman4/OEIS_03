\\ a(n) = (2*n)! * [x^(2*n)] (C(x)'' - 2*x*C(x)' - C(x)) / 2, where e.g.f. C(x) satisfies C(x) = cosh( Integral C(x)^3 dx ).
a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (2*n)!*polcoef((deriv(deriv(C))-2*x*deriv(C)-C)/2, 2*n);
for(n=0, 20, print1(a(n), ", "));

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); C;
a(10)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); deriv(C);
a(10)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); deriv(deriv(C));
a(10)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (deriv(deriv(C))-2*x*deriv(C)-C)/2;
a(10)