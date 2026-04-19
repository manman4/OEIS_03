\\ a(n) = (-1)^n * (1 + 9^n + 25^n + 81^n)/4 - Sum_{k=0..n-1} (-576)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * (1 + 9^n + 25^n + 81^n)/4 - sum(k=0, n-1, (-576)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 10, print1(a(n),", "));

a_vector(n, s=2, t=3, u=4) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i*((s+t+u)^(2*i)+(s+t-u)^(2*i)+(s-t+u)^(2*i)+(s-t-u)^(2*i))/4-sum(j=0, i-1, (-(s*t*u)^2)^(i-j)*binomial(2*i, 2*j)*v[j+1])); v;
a_vector(12)