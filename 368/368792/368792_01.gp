\\ a(n) = (2*n-1)!! * Sum_{k=0..n} k/(2*k-1)!!.

a001147(n) = prod(k=1, n, 2*k-1);
for(n=0, 20, print1(a001147(n), ", "))
a(n) = a001147(n)*sum(k=0, n, k/a001147(k));
for(n=0, 19, print1(a(n), ", "))