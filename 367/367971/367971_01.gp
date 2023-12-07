\\ a(0) = 1; a(n) = Sum_{k=1..n} ((k-1)! + (-1)^k) * binomial(n-1,k-1) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, ((k-1)!+(-1)^k)*binomial(n-1, k-1)*a(n-k)));
for(n=0, 19, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, ((j-1)!+(-1)^j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(22)