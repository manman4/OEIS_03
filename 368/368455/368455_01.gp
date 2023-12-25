\\ a(n) = (-1)^n + Sum_{k=1..n} 3^(k-1) * binomial(n,k) * a(n-k).
a(n) = (-1)^n + sum(k=1, n, 3^(k-1) * binomial(n, k) * a(n-k));
for(n=0, 19, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i+sum(j=1, i, 3^(j-1)*binomial(i, j)*v[i-j+1])); v;
a_vector(19)
 