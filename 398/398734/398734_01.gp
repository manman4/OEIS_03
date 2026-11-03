\\ a(0) = 1; a(n) = Sum_{k=1..n} binomial(2*n-1,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(2*n-1,k)*a(n-k)));
for(n=0, 15, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, binomial(2*i-1, j)*v[i-j+1])); v;
a_vector(18)
