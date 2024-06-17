\\ a(0) = 1; a(n) = ((n-1)!/120) * Sum_{k=5..n} k * a(n-k)/(n-k)!. 
a(n) = if(n==0, 1, ((n-1)!/120) * sum(k=5, n, k * a(n-k)/(n-k)! ));
for(n=0, 21, print1(a(n),", "))


