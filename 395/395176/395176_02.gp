\\ a(n) = (2*n)! * [x^(2*n)] C(x)^5, where C(x) satisfies C(x) = cosh( Integral C(x)^5 dx ).
a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^6 +x*O(x^(2*n))); C = 1 + intformal( S*C^5 ) ); (2*n)!*polcoeff(C^5, 2*n);
for(n=0, 15, print1(a(n),", "));

