\\ E.g.f. A(x) satisfies A(x) = x*exp(A^l(x)), where A^l(x) denotes the l-th iterate of A.
\\ Let a(n,k,l) = n! * [x^n] A^k(x), where A^k(x) is the k-th iterate of A.
\\ a(1,k,l) = 1; a(n,k,l) = (1/(n-1)) * Sum_{i=1..n-1} i * binomial(n,i) * a(n-i,k,l) * Sum_{j=1..k} a(i,j+l-1,l).
a(n, k, l) = if(n==1, 1, 1/(n-1) * sum(i=1, n-1, i * binomial(n,i) * a(n-i, k, l) * sum(j=1, k, a(i, j+l-1, l))) );

for(n=1, 8, print1(a(n, 1, 0), ", "));
for(n=1, 8, print1(a(n, 1, 1), ", "));
for(n=1, 8, print1(a(n, 1, 2), ", "));
for(n=1, 8, print1(a(n, 1, 3), ", "));
for(n=1, 8, print1(a(n, 1, 4), ", "));
for(n=1, 8, print1(a(n, 1, 5), ", "));
for(n=1, 8, print1(a(n, 1, 6), ", "));
