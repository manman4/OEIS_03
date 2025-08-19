\\ n*a(n) = (14*n-6)*a(n-1) - 13*n*a(n-2) for n > 1.
a(n) = my(v=[1, 8]); if(n<2, v[n+1], ((14*n-6)*a(n-1) - 13*n*a(n-2))/n);
for(n=0, 15, print1(a(n), ", "));

\\ Partial sums of A340973.
a340973(n) = sum(k=0, n, 3^k*binomial(n, k)*binomial(2*k, k));
for(n=0, 20, print1(sum(k=0, n, a340973(k)) - a(n), ", "));