\\ a(n) = (2*n)! * [x^(2*n)] C(x)^k, where e.g.f. C(x) satisfies C(x) = cosh( Integral C(x)^3 dx ).
a(n,k) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (2*n)! * polcoef(C^k, 2*n);
\\ matrix(6, 6, n, k, a(n-1, k-1))
cnt = 0
for(n=0, 9, for(k=0, n, print(cnt, " ", a(k, n-k)); cnt++));