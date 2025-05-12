\\ a(n) = Sum_{k=0..2*n} (-1)^k * Stirling2(k+5,5) * Stirling2(2*n-k+5,5).
a(n) = sum(k=0, 2*n, (-1)^k * stirling(k+5, 5, 2) * stirling(2*n-k+5, 5, 2));;
for(n=0, 18, print1(a(n),", "));
                  

