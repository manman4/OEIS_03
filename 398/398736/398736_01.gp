\\ a(0) = 1; a(n) = Sum_{k=1..n} binomial(5*n-1,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(5*n-1,k)*a(n-k)));
for(n=0, 15, print1(a(n), ", "));

\\ 4|a(n) for n > 0.
for(n=1, 15, print1(a(n)%4, ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, binomial(5*i-1, j)*v[i-j+1])); v;
a_vector(17)
