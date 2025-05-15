\\ A(n,k) = Sum_{j=0..n} Stirling2(j+k,k) * Stirling2(n-j+k,k).
a(n, k) = sum(j=0, n, stirling(j+k, k, 2) * stirling(n-j+k, k, 2));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));

print("Main diagonal gives A350376.");
for(n=0, 10, print1(a(n, n),", "));

print("A(n,n-1) gives A383880.");
for(n=1, 10, print1(a(n, n-1),", "));



