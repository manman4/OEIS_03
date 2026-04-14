a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (2*n)!*polcoef((deriv(deriv(C))-2*x*deriv(C)-C)/2, 2*n);
for(n=0, 20, print1(a(n), ", "));

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); C;
a(20)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); deriv(C);
a(20)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); deriv(deriv(C));
a(20)

a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (deriv(deriv(C))-2*x*deriv(C)-C)/2
a(20)