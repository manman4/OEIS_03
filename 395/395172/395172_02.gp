\\ A(n,k) = (2*n)! * [x^(2*n)] C(x)^k and C(x) satisfies C(x) = cosh( Integral C(x)^5 dx ).
a(n, k) = my(S=x, C=1); for(i=0, n, S = intformal( C^6 +x*O(x^(2*n))); C = 1 + intformal( S*C^5 ) ); (2*n)!*polcoef(C^k, 2*n)
for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));

