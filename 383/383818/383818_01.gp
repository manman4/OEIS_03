\\ A(n,k) = Sum_{j=0..k} |Stirling1(k,j)| * Stirling2(j+n,k).
a(n, k) = sum(j=0, k, abs(stirling(k, j, 1)) * stirling(j+n, k, 2));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));

print("Main diagonal gives A383839.");
for(n=0, 10, print1(a(n, n),", "));





