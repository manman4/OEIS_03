\\ a(n) = 7^n/n! * Product_{k=0..n-1} (7*k+5).
a(n) = 7^n/n! * prod(k=0, n-1, 7*k+5);
for(n=0, 20, print1(a(n), ", "));
