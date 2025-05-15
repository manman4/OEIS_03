\\ A(n,k) = Sum_{j=0..k} |Stirling1(k+1,j+1)| * Stirling2(j+n,k).
a(n, k) = sum(j=0, k, abs(stirling(k+1, j+1, 1)) * stirling(j+n, k, 2));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));

print("Main diagonal gives A350366.");
for(n=0, 10, print1(a(n, n),", "));

print("A(n,n-1) gives A383767.");
for(n=1, 10, print1(a(n, n-1),", "));



