a001541(n) = polchebyshev(n, 1, 3);
for(n=0, 12, print1(a001541(n),", "));
\\ a(n) = (-1)^n * A001541(n) - Sum_{k=0..n-1} (-4)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * a001541(n) - sum(k=0, n-1, (-4)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 12, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i*a001541(i)-sum(j=0, i-1, (-4)^(i-j)*binomial(2*i, 2*j)*v[j+1])); v;
a_vector(15)