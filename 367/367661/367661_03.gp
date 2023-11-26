M=18;
N=32;

a(n) = if(n<5, 2^n, a(n-5) + sum(k=0, n-1, a(k\5) * a(n-1-k))); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; v[2]=2; v[3]=4; v[4]=8; v[5]=16; for(i=5, n, v[i+1]=v[i-4]+sum(j=0, i-1, v[j\5+1]*v[i-j])); v;
a_vector(N)