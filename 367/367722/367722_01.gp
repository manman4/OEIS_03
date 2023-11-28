M=20;
N=21;

a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-1)\3, (-1)^k * (3*k+1) * a(k) * a(n-1-3*k) / (k! * (n-1-3*k)!) )); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=0, (i-1)\3, (-1)^j*(3*j+1)*v[j+1]*v[i-3*j]/(j!*(i-1-3*j)!))); v;
a_vector(N)