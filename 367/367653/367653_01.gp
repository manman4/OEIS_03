M=18;
N=34;

a(n) = if(n==0, 1, sum(k=0, n-1, a(k\4) * a(n-1-k))); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, v[j\4+1]*v[i-j])); v;
a_vector(N)