M=25;
N=57;

a(n) = (-1)^n + sum(k=0, (n-1)\4, a(k) * a(n-1-4*k)); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(-1)^i+sum(j=0, (i-1)\4, v[j+1]*v[i-4*j])); v;
a_vector(N)