\\ a(n) = (-1)^n * (0^n + 4^n + 16^n + 36^n)/4 - Sum_{k=0..n-1} (-36)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * (0^n + 4^n + 16^n + 36^n)/4 - sum(k=0, n-1, (-36)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 13, print1(a(n),", "));

a_vector(n, s=1, t=2, u=3) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i*((s+t+u)^(2*i)+(s+t-u)^(2*i)+(s-t+u)^(2*i)+(s-t-u)^(2*i))/4-sum(j=0, i-1, (-(s*t*u)^2)^(i-j)*binomial(2*i, 2*j)*v[j+1])); v;
a_vector(12)