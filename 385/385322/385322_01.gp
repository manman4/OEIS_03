M=19;

\\ a(0) = 1; a(n) = a(n-1) + (1/2) * Sum_{k=0..n-1} (1 + k) * k * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, a(n-1) + (1/2) * sum(k=0, n-1, (1 + k) * k * binomial(n-1,k) * a(k) * a(n-1-k)));
for(n=0, 12, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, i-1, (1+j)*j*binomial(i-1, j)*v[j+1]*v[i-j])/2); v;
for(n=0, M, print(a_vector(n)));