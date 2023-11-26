M=20;
N=32;

a(n) = if(n==0, 1, a(n-1) + sum(k=0, (n-1)\5, a(k) * a(n-1-5*k))); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, (i-1)\5, v[j+1]*v[i-5*j])); v;
a_vector(N)