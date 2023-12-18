\\ a(0) = 1; a(n) = 2*n*a(n-1) + Sum_{k=1..n} (-1)^(k-1) * (k-1)! * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, 2*n*a(n-1) + sum(k=1, n, (-1)^(k-1)*(k-1)!*binomial(n, k)*a(n-k)));
for(n=0, 15, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=2*i*v[i]+sum(j=1, i, (-1)^(j-1)*(j-1)!*binomial(i, j)*v[i-j+1])); v;
a_vector(17)
