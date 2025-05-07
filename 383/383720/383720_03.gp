pell(n) = ([2, 1; 1, 0]^n)[2, 1];
\\ a(n) = Pell(3*n)/Pell(n) for n > 0.
for(n=1, 20, print1(pell(3*n)/pell(n), ", "));