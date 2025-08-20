\\ n*a(n) = (6*n+3)*a(n-1) - 5*(n+1)*a(n-2) for n > 1.
a(n) = my(v=[1,9]); if(n<2, v[n+1], ((6*n+3)*a(n-1) - 5*(n+1)*a(n-2))/n);
for(n=0, 22, print1(a(n),", "));

\\ Partial sums of A383254.
a383254(n) = sum(k=0, n, (2*k+1)*binomial(2*k, k)*binomial(n+1, k+1));;
for(n=0, 22, print1(sum(k=0, n, a383254(k)) - a(n),", "));
