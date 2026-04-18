\\ a(n) = (-1)^n * ((3+sqrt(2))^(2*n) + (3-sqrt(2))^(2*n))/2 - Sum_{k=0..n-1} (-18)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * ((3+sqrt(2))^(2*n) + (3-sqrt(2))^(2*n))/2 - sum(k=0, n-1, (-18)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 12, print1(round(a(n)),", "));

a_vector(n, s=3, t=sqrt(2)) = my(v=vector(n+1)); for(i=0, n, v[i+1]=round((-1)^i*((s+t)^(2*i)+(s-t)^(2*i))/2-sum(j=0, i-1, (-(s*t)^2)^(i-j)*binomial(2*i, 2*j)*v[j+1]))); v;
a_vector(14);
