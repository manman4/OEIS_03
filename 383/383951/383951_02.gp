\\ n*a(n) = (14*n-6)*a(n-1) - 13*n*a(n-2) for n > 1.
a(n) = my(v=[1, 8]); if(n<2, v[n+1], ((14*n-6)*a(n-1) - 13*n*a(n-2))/n);
for(n=0, 15, print1(a(n), ", "));

\\ a(n) = A331516(n) - A331516(n-1).
a331516(n) = sum(k=1, n+1, 3^(n+1-k)*k*binomial(n+1, k)*binomial(n+1+k, k))/2;
for(n=0, 20, print1(a331516(n) - a331516(n-1) - a(n), ", "));