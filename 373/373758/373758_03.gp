\\ a(0) = 1; a(n) = ((n-1)!/24) * Sum_{k=4..n} k * (k-3) * a(n-k)/(n-k)!.
a(n) = if(n==0, 1, ((n-1)!/24) * sum(k=4, n, k * (k-3) * a(n-k)/(n-k)! )); 
for(n=0, 22, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!/24*sum(j=4, i, j*(j-3)*v[i-j+1]/(i-j)!)); v;
a_vector(21)
