\\ a(0) = 1; a(n) = ((n-1)!/6) * Sum_{k=3..n} k * (k-2) * a(n-k)/(n-k)!.
a(n) = if(n==0, 1, ((n-1)!/6) * sum(k=3, n, k * (k-2) * a(n-k)/(n-k)! ));  
for(n=0, 21, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!/6*sum(j=3, i, j*(j-2)*v[i-j+1]/(i-j)!)); v;
a_vector(21)
