\\ a(n) = (2*n-1)!! * Sum_{k=1..n} (-1)^(k-1)/(2*k-1)!!.

a001147(n) = prod(k=1, n, 2*k-1);
for(n=0, 20, print1(a001147(n), ", "))
a(n) = a001147(n)*sum(k=1, n, (-1)^(k-1)/a001147(k));
for(n=0, 20, print1(a(n), ", "))