M=18;
N=32;

a(n) = if(n<3, 2^n, a(n-3) + sum(k=0, n-1, a(k\3) * a(n-1-k))); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; v[2]=2; v[3]=4; for(i=3, n, v[i+1]=v[i-2]+sum(j=0, i-1, v[j\3+1]*v[i-j])); v;
a_vector(N)