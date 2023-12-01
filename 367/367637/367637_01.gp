M=25;
N=51;

\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/6)} a(k) * a(n-1-6*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\6, a(k) * a(n-1-6*k)));
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\6, v[j+1]*v[i-6*j])); v;
a_vector(N)