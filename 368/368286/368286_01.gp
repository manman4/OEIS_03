\\ a(n) = (-1)^n + 2 * Sum_{k=1..n} (k-1)! * binomial(n,k) * a(n-k).
a(n) = (-1)^n + 2 * sum(k=1, n, (k-1)!*binomial(n, k)*a(n-k));
for(n=0, 15, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i+2*sum(j=1, i, (j-1)!*binomial(i, j)*v[i-j+1])); v;
a_vector(18)