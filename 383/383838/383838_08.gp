\\ a(n) = Sum_{k=0..2*n} (-1)^k * Stirling2(k+4,4) * Stirling2(2*n-k+4,4).
a(n) = sum(k=0, 2*n, (-1)^k * stirling(k+4, 4, 2) * stirling(2*n-k+4, 4, 2));;
for(n=0, 18, print1(a(n),", "));
                  

