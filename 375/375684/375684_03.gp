\\ a(0) = 1; a(n) = -n! * Sum_{k=2..n} 1/(k-1) * a(n-k)/(n-k)!.
a(n) = if(n==0, 1, -n! * sum(k=2, n, 1/(k-1) * a(n-k)/(n-k)!))
for(n=0, 20, print1(a(n), ", "))
