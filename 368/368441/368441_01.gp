\\ a(n) = 1 + 3 * Sum_{k=1..n} 3^k * binomial(n,k) * a(n-k).
a(n) = 1 + 3 * sum(k=1, n, 3^k * binomial(n, k) * a(n-k));
for(n=0, 15, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=1+3*sum(j=1, i, 3^j*binomial(i, j)*v[i-j+1])); v;
a_vector(15)
 